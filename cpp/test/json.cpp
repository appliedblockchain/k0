#include <gtest/gtest.h>
#include <json/json.h>
#include "definitions.hpp"
#include "serialization.hpp"
#include "MerkleTree.hpp"
#include "pkutil.cpp"
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "scheme/comms.hpp"
#include "circuitry/CommitmentCircuit.hpp"
#include "json_conversion.hpp"

using namespace json_conversion;

TEST(JSON_serialization, Proofs)
{
  default_r1cs_ppzksnark_pp::init_public_params();
  bit_vector a_pk_bits = random_bits(256);
  bit_vector rho_bits = random_bits(256);
  bit_vector r_bits = random_bits(384);
  FieldT v = FieldT("1234");
  bit_vector v_bits = field_element_to_64_bits(v);

  auto k_bits = comm_r<CommitmentHashT>(a_pk_bits, rho_bits, r_bits);
  auto cm_bits = comm_s<CommitmentHashT>(k_bits, v_bits);
  auto circuit = make_commitment_circuit<FieldT, CommitmentHashT>();

  const auto keypair =
      r1cs_ppzksnark_generator<default_r1cs_ppzksnark_pp>(
          circuit.pb->get_constraint_system());

  circuit.k_bits->fill_with_bits(*circuit.pb, k_bits);
  circuit.k_packer->generate_r1cs_witness_from_bits();
  circuit.pb->val(*circuit.v_packed) = v;
  circuit.v_packer->generate_r1cs_witness_from_packed();
  assert(!circuit.pb->is_satisfied());

  circuit.commitment_gadget->generate_r1cs_witness();
  circuit.cm_packer->generate_r1cs_witness_from_bits();
  assert(circuit.pb->is_satisfied());
  assert(circuit.cm_bits->get_digest() == cm_bits);

  ASSERT_TRUE(circuit.pb->is_satisfied());

  const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
      r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
          keypair.pk, circuit.pb->primary_input(),
          circuit.pb->auxiliary_input());

  auto proof_as_json = proof_to_json_jacobian(proof);
  auto newproof = json_to_proof_jacobian(proof_as_json);

  ASSERT_EQ(proof, newproof);
  bool verified =
      r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
          keypair.vk, circuit.pb->primary_input(),
          newproof);
  ASSERT_TRUE(verified);

}