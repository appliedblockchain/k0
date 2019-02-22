#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>
#include "circuitry/MTAdditionCircuit.hpp"
#include "MerkleTree.hpp"
#include "util.h"

using namespace libff;
using namespace libsnark;
using namespace zktrade;

TEST(MerkleTreeAddition, Test) {

    typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
    typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;

    protoboard<FieldT> pb;

    const size_t tree_height = 1;

    MerkleTree mt(tree_height);

    for (size_t address = 0; address < exp2(tree_height); address++) {
        cout << "Address " << dec << address << endl;
        auto address_bits = int_to_bits<FieldT>(address, tree_height);
        const auto leaf = random_bits(256);
        auto sim = mt.simulate_add(leaf);
        auto c = make_mt_addition_circuit<FieldT>(tree_height);
        c.prev_root_bits->generate_r1cs_witness(mt.root());
        c.address_bits->fill_with_bits(*c.pb, address_bits);
        c.prev_path_var->generate_r1cs_witness(address, mt.path(address));
        c.next_leaf_bits->generate_r1cs_witness(leaf);
        c.next_root_bits->generate_r1cs_witness(get<1>(sim));
        c.next_path_var->generate_r1cs_witness(address, get<2>(sim));

        // How can prev_path and next_path differ anyway?
        ASSERT_EQ(mt.path(address), get<2>(sim));

        ASSERT_FALSE(c.pb->is_satisfied());
        c.mt_update_gadget->generate_r1cs_witness();
        ASSERT_TRUE(c.pb->is_satisfied());

        // Check that no input got overwritten
        ASSERT_EQ(c.address_bits->get_bits(*c.pb), address_bits);
        ASSERT_EQ(c.prev_leaf_bits->get_digest(), zero_bits(256));
        ASSERT_EQ(c.prev_root_bits->get_digest(), mt.root());
        ASSERT_EQ(c.prev_path_var->get_authentication_path(address),
                  mt.path(address));
        ASSERT_EQ(c.next_leaf_bits->get_digest(), leaf);
        ASSERT_EQ(c.next_root_bits->get_digest(), get<1>(sim));
        ASSERT_EQ(c.next_path_var->get_authentication_path(address),
                  get<2>(sim));

        mt.add(leaf);
    }


}
