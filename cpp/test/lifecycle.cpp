#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/gadgets/dummyhash_gadget.hpp"
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/MTAdditionCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "circuitry/NewTransferCircuit.hpp"
#include "MerkleTree.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"
#include "util.h"

using namespace zktrade;
using namespace libsnark;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

typedef sha256_compression_gadget<FieldT> CommitmentHashT;
typedef sha256_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

TEST(Lifecycle, Full) {

    struct coin {
        size_t address;
        bit_vector rho;
        bit_vector r;
        bit_vector a_sk;
        uint64_t v;
    };

    vector<coin> coins;

    size_t tree_height = 3;

    MerkleTree<MerkleTreeHashT> mt(tree_height);

    size_t num_initial_coins = exp2(tree_height) / 2;
//    size_t num_initial_coins = exp2(tree_height);

    for (size_t address = 0; address < num_initial_coins; address++) {

        // Sample private key
        auto a_sk = random_bits(256);
        // Derive public key/address
        auto a_pk = prf_addr<CommitmentHashT>(a_sk);

        // DEPOSIT/"SHIELDING"
        // Sample rho
        auto rho = random_bits(256);
        // Sample r
        auto r = random_bits(384);
        // Generate k
        auto k = comm_r<CommitmentHashT>(a_pk, rho, r);

        // Set v
        uint64_t v_uint = 5000000000000000000;
        string v_str = to_string(v_uint);
        FieldT v = FieldT(v_str.c_str());
        bit_vector v_bits = uint64_to_bits(stoull(v_str));

        coin c{address, rho, r, a_sk, v_uint};
        coins.push_back(c);

        // Generate cm
        auto cm = comm_s<CommitmentHashT>(k, v_bits);

        ASSERT_EQ(mt.num_elements(), address);

        bit_vector address_bits = int_to_bits<FieldT>(address, tree_height);
        // Add cm to the tree
        auto sim_result = mt.simulate_add(cm);


        // Generate deposit proof
        auto comm_circuit = make_commitment_circuit<FieldT, CommitmentHashT>();
        comm_circuit.k_bits->fill_with_bits(*comm_circuit.pb, k);
        comm_circuit.k_packer->generate_r1cs_witness_from_bits();
        comm_circuit.pb->val(*comm_circuit.v_packed) = v;
        comm_circuit.v_packer->generate_r1cs_witness_from_packed();
        ASSERT_FALSE(comm_circuit.pb->is_satisfied());

        comm_circuit.commitment_gadget->generate_r1cs_witness();
        comm_circuit.cm_packer->generate_r1cs_witness_from_bits();
        ASSERT_TRUE(comm_circuit.pb->is_satisfied());
        ASSERT_EQ(comm_circuit.cm_bits->get_digest(), cm);

        auto cm_packed = comm_circuit.cm_packed->get_vals(*comm_circuit.pb);

        auto add_circuit = make_mt_addition_circuit<FieldT, MerkleTreeHashT>(
                tree_height);

        add_circuit.prev_root_bits->generate_r1cs_witness(mt.root());
        add_circuit.prev_root_packer->generate_r1cs_witness_from_bits();

        add_circuit.pb->val(*add_circuit.address_packed) = address;
        add_circuit.address_packer->generate_r1cs_witness_from_packed();

        add_circuit.prev_path_var->generate_r1cs_witness(address,
                                                         mt.path(address));

        add_circuit.next_leaf_packed->fill_with_field_elements(*add_circuit.pb,
                                                               cm_packed);
        add_circuit.next_leaf_packer->generate_r1cs_witness_from_packed();
        ASSERT_EQ(add_circuit.next_leaf_bits->get_digest(), cm);

        add_circuit.next_root_bits->generate_r1cs_witness(get<1>(sim_result));
        add_circuit.next_root_packer->generate_r1cs_witness_from_bits();

        ASSERT_FALSE(add_circuit.pb->is_satisfied());
        add_circuit.mt_update_gadget->generate_r1cs_witness();

        ASSERT_TRUE(add_circuit.pb->is_satisfied());

        cout << "Root before " << bits2hex(mt.root()) << endl;
        //mt.print();
        cout << "Adding      " << bits2hex(cm) << endl;
        mt.add(cm);
        cout << "Root after  " << bits2hex(mt.root()) << endl;
        //mt.print();
    }


    // TRANSFER
    for (size_t input_0_address = 0; input_0_address < num_initial_coins; input_0_address += 2) {

        cout << "Address " << input_0_address << endl;

        coin c = coins[input_0_address];
        bit_vector address_bits = int_to_bits<FieldT>(c.address, tree_height);
        string v_str = to_string(c.v);
        FieldT v = FieldT(v_str.c_str());

        // Generate recipient address
        uint32_t recipient = rand() % (uint32_t)exp2(20);

        // Generate proof
        auto wd_circuit = make_withdrawal_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(tree_height);
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.recipient_public) = recipient;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(input_0_address, mt.path(input_0_address));
        wd_circuit.pb->val(*wd_circuit.recipient_private) = recipient;
        ASSERT_FALSE(wd_circuit.pb->is_satisfied());
        wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
        wd_circuit.v_packer->generate_r1cs_witness_from_packed();
        cout << "BEFORE" << endl;
        cout << "Root " << bits2hex(wd_circuit.rt_bits->get_digest()) << endl;
        cout << "a_sk          " << bits2hex(wd_circuit.a_sk_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "rho          " << bits2hex(wd_circuit.rho_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "r          " << bits2hex(wd_circuit.r_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "v          " << bits2hex(wd_circuit.v_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "cm          " << bits2hex(wd_circuit.commitment_bits->get_digest()) << endl;
        cout << "Path" << endl;

        wd_circuit.note_gadget->generate_r1cs_witness();
        wd_circuit.sn_packer->generate_r1cs_witness_from_bits();

        cout << "AFTER" << endl;
        cout << "Root " << bits2hex(wd_circuit.rt_bits->get_digest()) << endl;
        cout << "a_sk          " << bits2hex(wd_circuit.a_sk_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "rho          " << bits2hex(wd_circuit.rho_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "r          " << bits2hex(wd_circuit.r_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "v          " << bits2hex(wd_circuit.v_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "cm          " << bits2hex(wd_circuit.commitment_bits->get_digest()) << endl;
        cout << "Path" << endl;
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());

        // Set original inputs again to make sure nothing has been overwritten
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(input_0_address, mt.path(input_0_address));
        // Circuit should still be satisfied
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());
    }

    for (size_t address = 0; address < num_initial_coins; address++) {
        // WITHDRAWAL/"UNSHIELDING"

        cout << "Address " << address << endl;

        coin c = coins[address];
        bit_vector address_bits = int_to_bits<FieldT>(c.address, tree_height);
        string v_str = to_string(c.v);
        FieldT v = FieldT(v_str.c_str());

        // Generate recipient address
        uint32_t recipient = rand() % (uint32_t)exp2(20);

        // Generate proof
        auto wd_circuit = make_withdrawal_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(tree_height);
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.recipient_public) = recipient;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        wd_circuit.pb->val(*wd_circuit.recipient_private) = recipient;
        ASSERT_FALSE(wd_circuit.pb->is_satisfied());
        wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
        wd_circuit.v_packer->generate_r1cs_witness_from_packed();
                cout << "BEFORE" << endl;
        cout << "Root " << bits2hex(wd_circuit.rt_bits->get_digest()) << endl;
        cout << "a_sk          " << bits2hex(wd_circuit.a_sk_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "rho          " << bits2hex(wd_circuit.rho_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "r          " << bits2hex(wd_circuit.r_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "v          " << bits2hex(wd_circuit.v_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "cm          " << bits2hex(wd_circuit.commitment_bits->get_digest()) << endl;
        cout << "Path" << endl;

        wd_circuit.note_gadget->generate_r1cs_witness();
        wd_circuit.sn_packer->generate_r1cs_witness_from_bits();

        cout << "AFTER" << endl;
        cout << "Root " << bits2hex(wd_circuit.rt_bits->get_digest()) << endl;
        cout << "a_sk          " << bits2hex(wd_circuit.a_sk_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "rho          " << bits2hex(wd_circuit.rho_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "r          " << bits2hex(wd_circuit.r_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "v          " << bits2hex(wd_circuit.v_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "cm          " << bits2hex(wd_circuit.commitment_bits->get_digest()) << endl;
        cout << "Path" << endl;
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());

        // Set original inputs again to make sure nothing has been overwritten
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        // Circuit should still be satisfied
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());
    }
}
