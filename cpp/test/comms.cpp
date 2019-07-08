#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/gadgets/comms.hpp"
#include "scheme/comms.hpp"

using namespace libff;
using namespace libsnark;
using namespace std;
using namespace k0;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_compression_gadget<FieldT> HashT;

TEST(comms, comm_r) {
    // Scheme result
    bit_vector a_pk_bv = random_bits(256);
    bit_vector rho_bv = random_bits(256);
    bit_vector r_bv = random_bits(384);
    bit_vector k_bv = comm_r<HashT>(a_pk_bv, rho_bv, r_bv);

    // Gadget result
    protoboard<FieldT> pb;
    pb_variable_array<FieldT> a_pk_va;
    a_pk_va.allocate(pb, 256, "a_pk");
    pb_variable_array<FieldT> rho_va;
    rho_va.allocate(pb, 256, "rho");
    pb_variable_array<FieldT> r_va;
    r_va.allocate(pb, 384, "r");
    digest_variable<FieldT> k_dv(pb, 256, "k");
    comm_r_gadget<FieldT, HashT> g(pb, a_pk_va, rho_va, r_va, k_dv,
                                   "comm_r_gadget");
    k_dv.generate_r1cs_constraints();
    g.generate_r1cs_constraints();

    a_pk_va.fill_with_bits(pb, a_pk_bv);
    rho_va.fill_with_bits(pb, rho_bv);
    r_va.fill_with_bits(pb, r_bv);
    g.generate_r1cs_witness();

    ASSERT_EQ(k_dv.get_digest(), k_bv);
}

TEST(comms, comm_s) {
    // Scheme result
    bit_vector k_bv = random_bits(256);
    bit_vector v_bv = random_bits(64);
    bit_vector cm_bv = comm_s<HashT>(k_bv, v_bv);

    // Gadget result
    protoboard<FieldT> pb;
    pb_variable<FieldT> ZERO;
    ZERO.allocate(pb, "ZERO");
    pb_variable_array<FieldT> k_va;
    k_va.allocate(pb, 256, "k");
    pb_variable_array<FieldT> v_va;
    v_va.allocate(pb, 64, "v");
    digest_variable<FieldT> cm_dv(pb, 256, "cm");
    comm_s_gadget<FieldT, HashT> g(pb, ZERO, k_va, v_va, cm_dv,
                                   "comm_s_gadget");
    cm_dv.generate_r1cs_constraints();
    g.generate_r1cs_constraints();

    pb.val(ZERO) = FieldT::zero();
    k_va.fill_with_bits(pb, k_bv);
    v_va.fill_with_bits(pb, v_bv);
    g.generate_r1cs_witness();

    ASSERT_EQ(cm_dv.get_digest(), cm_bv);
}

TEST(comms, cm_full) {
    // Scheme result
    bit_vector a_pk_bv = random_bits(256);
    bit_vector rho_bv = random_bits(256);
    bit_vector r_bv = random_bits(384);
    bit_vector k_bv = comm_r<HashT>(a_pk_bv, rho_bv, r_bv);
    bit_vector v_bv = random_bits(64);
    bit_vector cm_bv = comm_s<HashT>(k_bv, v_bv);

    // Gadget result
    protoboard<FieldT> pb;
    pb_variable<FieldT> ZERO;
    ZERO.allocate(pb, "ZERO");
    pb_variable_array<FieldT> a_pk_va;
    a_pk_va.allocate(pb, 256, "a_pk");
    pb_variable_array<FieldT> rho_va;
    rho_va.allocate(pb, 256, "rho");
    pb_variable_array<FieldT> r_va;
    r_va.allocate(pb, 384, "r");
    pb_variable_array<FieldT> v_va;
    v_va.allocate(pb, 64, "v");
    digest_variable<FieldT> cm_dv(pb, 256, "cm");
    cm_full_gadget<FieldT, HashT> g(pb, ZERO, a_pk_va, rho_va, r_va, v_va,
                                    cm_dv, "cm_full_gadget");
    cm_dv.generate_r1cs_constraints();
    g.generate_r1cs_constraints();

    a_pk_va.fill_with_bits(pb, a_pk_bv);
    rho_va.fill_with_bits(pb, rho_bv);
    r_va.fill_with_bits(pb, r_bv);
    v_va.fill_with_bits(pb, v_bv);
    g.generate_r1cs_witness();

    ASSERT_EQ(g.k_bits(), k_bv);
    ASSERT_EQ(cm_dv.get_digest(), cm_bv);
}