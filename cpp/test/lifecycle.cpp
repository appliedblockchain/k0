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

    struct coin {
        size_t address;
        bit_vector rho;
        bit_vector r;
        bit_vector a_sk;
        uint64_t v;
    };

    vector<coin> coins;

    size_t tree_height = 3;

    MerkleTree<TwoToOneHashT> mt(tree_height);

    size_t num_initial_coins = exp2(tree_height) / 2;

    for (size_t address = 0; address < num_initial_coins; address++) {

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
        uint64_t v_uint = 5000000000000000000;
        string v_str = to_string(v_uint);
        FieldT v = FieldT(v_str.c_str());
        bit_vector v_bits = uint64_to_bits(stoull(v_str));

        coin c{address, rho, r, a_sk, v_uint};
        coins.push_back(c);

        // Generate cm
        auto cm = comm_s<CompressionHashT>(k, v_bits);

        ASSERT_EQ(mt.num_elements(), address);

        bit_vector address_bits = int_to_bits<FieldT>(address, tree_height);
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

        auto add_circuit = make_mt_addition_circuit<FieldT, TwoToOneHashT>(
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

        mt.add(cm);
    }


    // TRANSFER
    for (size_t input_0_address = 0; input_0_address < num_initial_coins; input_0_address += 2) {

        auto circuit = make_transfer_circuit<FieldT, CompressionHashT, TwoToOneHashT>(tree_height);

        circuit.rt_bits->generate_r1cs_witness(mt.root());

        // input coins
        for (size_t i = 0; i < 2; i++) {
            coin c = coins[input_0_address + i];
            bit_vector address_bits = int_to_bits<FieldT>(c.address, tree_height);
            bit_vector v_bits = int_to_bits<FieldT>(c.v, 64);

            circuit.address_in_bits_vec[i]->fill_with_bits(*circuit.pb, address_bits);
            circuit.path_in_vec[i]->generate_r1cs_witness(c.address, mt.path(c.address));
            circuit.a_sk_in_bits_vec[i]->fill_with_bits(*circuit.pb, c.a_sk);
            circuit.rho_in_bits_vec[i]->fill_with_bits(*circuit.pb, c.rho);
            circuit.r_in_bits_vec[i]->fill_with_bits(*circuit.pb, c.r);
            circuit.v_in_bits_vec[i]->fill_with_bits(*circuit.pb, v_bits);

            circuit.input_note_vec[i]->generate_r1cs_witness();
            circuit.sn_in_packer_vec[i]->generate_r1cs_witness_from_bits();
        }

        // output coins
        for (size_t i = 0; i < 2; i++) {

            size_t address = num_initial_coins + input_0_address + i;

            auto a_sk = random_bits(256);
            auto a_pk = prf_addr<CompressionHashT>(a_sk);
            auto rho = random_bits(256);
            auto r = random_bits(384);

            uint64_t v_uint = 5000000000000000000;
            string v_str = to_string(v_uint);
            FieldT v = FieldT(v_str.c_str());
            bit_vector v_bits = uint64_to_bits(stoull(v_str));

            coin c{address, rho, r, a_sk, v_uint};
            coins.push_back(c);

            circuit.a_pk_out_bits_vec[i]->fill_with_bits(*circuit.pb, a_pk);
            circuit.rho_out_bits_vec[i]->fill_with_bits(*circuit.pb, rho);
            circuit.r_out_bits_vec[i]->fill_with_bits(*circuit.pb, r);
            circuit.v_out_bits_vec[i]->fill_with_bits(*circuit.pb, v_bits);
            circuit.cm_out_gadget_vec[i]->generate_r1cs_witness();
            circuit.cm_out_packer_vec[i]->generate_r1cs_witness_from_bits();
        }

        circuit.rt_packer->generate_r1cs_witness_from_bits();
        ASSERT_TRUE(circuit.pb->is_satisfied());

        for (size_t i = 0; i < 2; i++) {
            mt.add(circuit.cm_out_bits_vec[i]->get_digest());
        }

    }

    for (size_t address = 0; address < num_initial_coins * 2; address++) {
        // WITHDRAWAL/"UNSHIELDING"

        coin c = coins[address];
        bit_vector address_bits = int_to_bits<FieldT>(c.address, tree_height);
        string v_str = to_string(c.v);
        FieldT v = FieldT(v_str.c_str());

        // Generate recipient address
        uint32_t recipient = rand() % (uint32_t)exp2(20);

        // Generate proof
        auto wd_circuit = make_withdrawal_circuit<FieldT, CompressionHashT, TwoToOneHashT>(tree_height);
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
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        // Circuit should still be satisfied
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());
    }
}
