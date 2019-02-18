#include <gtest/gtest.h>
#include <libff/common/utils.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include "circuitry/gadgets/cm.h"
#include "circuitry/gadgets/sha256_compression.h"
#include "circuitry/OuterNoteCommitmentCircuit.hpp"
#include "scheme/prfs.h"
#include "scheme/cm.h"
#include "util.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

TEST(NoteCommitment, Test) {

    for (size_t i = 0; i < 1; i++) {

        uint64_t v = random_uint64();
        bit_vector a_sk_bv = random_bits(256);
        bit_vector a_pk_bv = prf_addr(a_sk_bv);
        bit_vector rho_bv = random_bits(256);
        bit_vector r_bv = random_bits(384);
        bit_vector v_bv = uint64_to_bits(v);
        bit_vector cm_bv = cm(a_pk_bv, rho_bv, r_bv, v_bv);

        protoboard<FieldT> pb;
        pb_variable<FieldT> ZERO_v;
        ZERO_v.allocate(pb, "ZERO");
        pb_variable_array<FieldT> a_pk_v;
        a_pk_v.allocate(pb, 256, "a_pk");
        pb_variable_array<FieldT> rho_v;
        rho_v.allocate(pb, 256, "rho");
        pb_variable_array<FieldT> r_v;
        r_v.allocate(pb, 384, "r");
        pb_variable_array<FieldT> v_v;
        v_v.allocate(pb, 64, "v");

        digest_variable<FieldT> cm_dv(pb, 256, "result");
        cm_gadget<FieldT> cmg(pb, ZERO_v, a_pk_v, rho_v, r_v, v_v, cm_dv);
        cmg.generate_r1cs_constraints();
        pb.val(ZERO_v) = FieldT::zero();
        a_pk_v.fill_with_bits(pb, a_pk_bv);
        rho_v.fill_with_bits(pb, rho_bv);
        r_v.fill_with_bits(pb, r_bv);
        v_v.fill_with_bits(pb, v_bv);

        ASSERT_FALSE(pb.is_satisfied());
        cmg.generate_r1cs_witness();
        ASSERT_TRUE(pb.is_satisfied());
        ASSERT_EQ(cm_dv.get_digest(), cm_bv);


        auto circuit = make_outer_note_commitment_circuit<FieldT>();
        circuit.pb->val(*circuit.v_packed) = FieldT(v);
        circuit.k_bits->fill_with_bits(*circuit.pb, cmg.k_bits());
        ASSERT_FALSE(circuit.pb->is_satisfied());
        circuit.v_packer->generate_r1cs_witness_from_packed();
        circuit.ocmg->generate_r1cs_witness();
        circuit.cm_packer->generate_r1cs_witness_from_bits();
        circuit.k_packer->generate_r1cs_witness_from_bits();
        ASSERT_TRUE(circuit.pb->is_satisfied());

    }
}

TEST(NoteCommitment, OuterCommitment) {
    auto circuit = make_outer_note_commitment_circuit<FieldT>();

    auto k_bv = random_bits(256);
    circuit.pb->val(*circuit.v_packed) = FieldT("5");
    circuit.k_bits->fill_with_bits(*circuit.pb, k_bv);
    ASSERT_FALSE(circuit.pb->is_satisfied());
    circuit.v_packer->generate_r1cs_witness_from_packed();
    circuit.ocmg->generate_r1cs_witness();
    circuit.cm_packer->generate_r1cs_witness_from_bits();
    circuit.k_packer->generate_r1cs_witness_from_bits();
    ASSERT_TRUE(circuit.pb->is_satisfied());
}

