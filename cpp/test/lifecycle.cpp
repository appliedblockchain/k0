#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/gadgets/dummyhash_gadget.hpp"
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/MTAdditionCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "circuitry/TransferCircuit.hpp"
#include "MerkleTree.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"
#include "util.h"

using namespace zktrade;
using namespace libsnark;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef zktrade::dummyhash_two_to_one_hash_gadget<FieldT> TwoToOneHashT;
typedef zktrade::dummyhash_compression_gadget<FieldT> CompressionHashT;

TEST(Lifecycle, Full) {

    size_t tree_height = 2;

    MerkleTree<TwoToOneHashT> mt(tree_height);

    for (size_t address = 0; address < exp2(tree_height); address++) {
        cout << endl << "Address " << address << endl;

        // Sample private key
        auto a_sk = random_bits(256);
        // Derive public key/address
        auto a_pk = prf_addr<CompressionHashT>(a_sk);

        // DEPOSIT/"SHIELDING"
        // Sample rho
        auto rho = random_bits(256);
        // Sample r
        auto r = random_bits(384);
        // Generate k
        auto k = comm_r<CompressionHashT>(a_pk, rho, r);
        // Set v
        string v_str = "5000000000000000000";
        FieldT v = FieldT(v_str.c_str());
        bit_vector v_bits = uint64_to_bits(stoull(v_str));
        // Generate cm
        auto cm = comm_s<CompressionHashT>(k, v_bits);

        ASSERT_EQ(mt.num_elements(), address);

        bit_vector address_bits = int_to_bits<FieldT>(address, tree_height);
        cout << "Address bits " << bits_to_string(address_bits) << endl;
        // Add cm to the tree
        auto sim_result = mt.simulate_add(cm);


        // Generate deposit proof
        auto comm_circuit = make_commitment_circuit<FieldT, CompressionHashT>();
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

        auto add_circuit = make_mt_addition_circuit<FieldT, TwoToOneHashT>(tree_height);

        add_circuit.prev_root_bits->generate_r1cs_witness(mt.root());
        add_circuit.prev_root_packer->generate_r1cs_witness_from_bits();

        add_circuit.pb->val(*add_circuit.address_packed) = address;
        add_circuit.address_packer->generate_r1cs_witness_from_packed();

        add_circuit.prev_path_var->generate_r1cs_witness(address, mt.path(address));

        add_circuit.next_leaf_packed->fill_with_field_elements(*add_circuit.pb, cm_packed);
        add_circuit.next_leaf_packer->generate_r1cs_witness_from_packed();
        ASSERT_EQ(add_circuit.next_leaf_bits->get_digest(), cm);

        add_circuit.next_root_bits->generate_r1cs_witness(get<1>(sim_result));
        add_circuit.next_root_packer->generate_r1cs_witness_from_bits();

        ASSERT_FALSE(add_circuit.pb->is_satisfied());
        add_circuit.mt_update_gadget->generate_r1cs_witness();

        ASSERT_TRUE(add_circuit.pb->is_satisfied());

        mt.add(cm);


        // TRANSFER
        auto transfer_circuit = make_transfer_circuit<FieldT, CompressionHashT, TwoToOneHashT>(tree_height);




        // WITHDRAWAL/"UNSHIELDING"

        // Generate recipient address
        uint32_t recipient = rand() % (uint32_t)exp2(20);

        // Generate proof
        auto wd_circuit = make_withdrawal_circuit<FieldT, CompressionHashT, TwoToOneHashT>(tree_height);
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.recipient_public) = recipient;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        wd_circuit.pb->val(*wd_circuit.recipient_private) = recipient;
        ASSERT_FALSE(wd_circuit.pb->is_satisfied());
        wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
        wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
        wd_circuit.v_packer->generate_r1cs_witness_from_packed();
        wd_circuit.addr_gadget->generate_r1cs_witness();
        wd_circuit.commitment_gadget->generate_r1cs_witness();
        wd_circuit.sn_gadget->generate_r1cs_witness();
        wd_circuit.sn_packer->generate_r1cs_witness_from_bits();
        wd_circuit.mt_path_gadget->generate_r1cs_witness();
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());

        // Set original inputs again to make sure nothing has been overwritten
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        // Circuit should still be satisfied
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());
    }
}
