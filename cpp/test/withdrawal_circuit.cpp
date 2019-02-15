#include <gtest/gtest.h>
#include <libff/common/utils.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <scheme/cm.h>
#include <MerkleTree.hpp>
#include "circuitry/WithdrawalCircuit.hpp"
#include "util.h"
#include "scheme/prfs.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

TEST(WithdrawalCircuit, Test) {
    size_t tree_height = 2;

    bit_vector a_sk = random_bits(256);
    bit_vector a_pk = prf_addr(a_sk);
    bit_vector rho = random_bits(256);
    bit_vector r = random_bits(384);
    FieldT v_field_element = FieldT("1000000000000000000");
    bit_vector v = field_element_to_64_bits(v_field_element);
    cout << "bitcoin v bits " << bits_to_hex(v) << endl;
    bit_vector commitment = cm(a_pk, rho, r, v);
    bit_vector sn = prf_sn(a_sk, rho);
    // TODO calculate sn

    MerkleTree mt(tree_height);
    uint address = mt.num_elements();
    const libff::bit_vector address_bits = libff::convert_field_element_to_bit_vector<FieldT>(FieldT(address, true), tree_height);
    mt.add(commitment);

    WithdrawalCircuit<FieldT> circuit = make_withdrawal_circuit<FieldT>(tree_height);
    circuit.root_bits->generate_r1cs_witness(mt.root());
    circuit.address_bits->fill_with_bits(*circuit.pb, address_bits);
    circuit.a_sk_bits->fill_with_bits(*circuit.pb, a_sk);
    circuit.rho_bits->fill_with_bits(*circuit.pb, rho);
    circuit.r_bits->fill_with_bits(*circuit.pb, r);
    circuit.pb->val(*circuit.v_packed) = v_field_element;

    circuit.root_packer->generate_r1cs_witness_from_bits();
    circuit.v_packer->generate_r1cs_witness_from_packed();
    ASSERT_FALSE(circuit.pb->is_satisfied());
    cout << "circuit " << bits_to_hex(circuit.commitment_bits->get_digest()) << endl;
    circuit.addr_gadget->generate_r1cs_witness();
    circuit.commitment_gadget->generate_r1cs_witness();
    circuit.sn_gadget->generate_r1cs_witness();
    circuit.sn_packer->generate_r1cs_witness_from_bits();
    cout << "bitcoin " << bits_to_hex(commitment) << endl;
    cout << "circuit " << bits_to_hex(circuit.commitment_bits->get_digest()) << endl;
    ASSERT_FALSE(circuit.pb->is_satisfied());
    circuit.path->generate_r1cs_witness(address, mt.path(address));
    circuit.mt_path_gadget->generate_r1cs_witness();

    ASSERT_TRUE(circuit.pb->is_satisfied());
    ASSERT_EQ(circuit.commitment_bits->get_digest(), commitment);
    ASSERT_EQ(circuit.a_pk_bits->get_digest(), a_pk);
    ASSERT_EQ(circuit.sn_bits->get_digest(), sn);
    cout << bits_to_hex(sn) << endl;
}
