#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/DepositCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "MerkleTree.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"
#include "util.h"

using namespace zktrade;
using namespace libsnark;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneHashT;

TEST(Lifecycle, Full) {

    size_t tree_height = 2;

    MerkleTree mt(tree_height);

    for (size_t address = 0; address < exp2(tree_height); address++) {
        cout << endl << "Address " << address << endl;

        // Sample private key
        auto a_sk = random_bits(256);
        // Derive public key/address
        auto a_pk = prf_addr(a_sk);
        // "DEPOSIT/SHIELDING"
        // Sample rho
        auto rho = random_bits(256);
        // Sample r
        auto r = random_bits(384);
        // Generate k
        auto k = comm_r(a_pk, rho, r);
        // Set v
        string v_str = "5000000000000000000";
        FieldT v = FieldT(v_str.c_str());
        bit_vector v_bits = uint64_to_bits(stoull(v_str));
        // Generate cm
        auto cm = comm_s(k, v_bits);

        ASSERT_EQ(mt.num_elements(), address);

        bit_vector address_bits = int_to_bits<FieldT>(address, tree_height);
        cout << "Address bits " << bits_to_string(address_bits) << endl;
        // Add cm to the tree
        auto sim_result = mt.simulate_add(cm);


        // Generate deposit proof
        auto dep_circuit = make_deposit_circuit<FieldT>(tree_height);
        dep_circuit.prev_root_bits->generate_r1cs_witness(mt.root());

        dep_circuit.pb->val(*dep_circuit.address_packed) = address;
        dep_circuit.k_bits->fill_with_bits(*dep_circuit.pb, k);
        dep_circuit.k_packer->generate_r1cs_witness_from_bits();
        dep_circuit.pb->val(*dep_circuit.v_packed) = v;
        dep_circuit.v_packer->generate_r1cs_witness_from_packed();

        dep_circuit.commitment_gadget->generate_r1cs_witness();
        dep_circuit.cm_packer->generate_r1cs_witness_from_bits();
        // TODO pack k locally and compare
        // TODO compare cm with one created above
        // TODO pack cm locally and compare

        dep_circuit.next_root_bits->generate_r1cs_witness(get<1>(sim_result));
        dep_circuit.path_var->generate_r1cs_witness(address,
                                                    get<2>(sim_result));
        dep_circuit.mt_addition_gadget->generate_r1cs_witness();
        ASSERT_FALSE(dep_circuit.pb->is_satisfied());

        dep_circuit.prev_root_packer->generate_r1cs_witness_from_bits();
        dep_circuit.next_root_packer->generate_r1cs_witness_from_bits();

        auto cm_packed = dep_circuit.cm_packed->get_vals(*dep_circuit.pb);
        ASSERT_TRUE(dep_circuit.pb->is_satisfied());
        ASSERT_EQ(dep_circuit.cm_bits->get_digest(), cm);

        mt.add(cm);

        // "WITHDRAWAL/UNSHIELDING"

        // Generate recipient address
        uint32_t recipient = rand() % (uint32_t)exp2(20);

        // Generate proof
        auto wd_circuit = make_withdrawal_circuit<FieldT>(tree_height);
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
