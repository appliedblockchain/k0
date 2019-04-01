#include "definitions.hpp"
#include "serialization.hpp"
#include "util.h"
#include "scheme/comms.hpp"
#include "circuitry/CommitmentCircuit.hpp"
#include "pkutil.cpp"
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

int main(int argc, char* argv[])
{
  default_r1cs_ppzksnark_pp::init_public_params();
  cout << "Loading pk...";
  auto commitment_pk =
  loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
        argv[1]);
  cout << "done." << endl;

  cout << "Loading vk...";
  auto commitment_vk =
  loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
        argv[2]);
  cout << "done." << endl;

    bit_vector a_pk_bits = random_bits(256);
    bit_vector rho_bits = random_bits(256);
    bit_vector r_bits = random_bits(384);
    FieldT v = FieldT("1234");
    bit_vector v_bits = field_element_to_64_bits(v);

    auto k_bits = comm_r<CommitmentHashT>(a_pk_bits, rho_bits, r_bits);
    auto cm_bits = comm_s<CommitmentHashT>(k_bits, v_bits);
    auto circuit = make_commitment_circuit<FieldT, CommitmentHashT>();

    // Generate deposit proof
    circuit.k_bits->fill_with_bits(*circuit.pb, k_bits);
    circuit.k_packer->generate_r1cs_witness_from_bits();
    circuit.pb->val(*circuit.v_packed) = v;
    circuit.v_packer->generate_r1cs_witness_from_packed();
    assert(!circuit.pb->is_satisfied());

    circuit.commitment_gadget->generate_r1cs_witness();
    circuit.cm_packer->generate_r1cs_witness_from_bits();
    assert(circuit.pb->is_satisfied());
    assert(circuit.cm_bits->get_digest() == cm_bits);

    if (!circuit.pb->is_satisfied()) {
      return 1;
    }

    cout << "CIRCUIT" << endl;
    cout << "k  " << bits2hex(circuit.k_bits->get_bits(*circuit.pb))
         << endl;
    cout << "cm " << bits2hex(circuit.cm_bits->get_digest()) << endl;
    cout << "CHECKING COMMITMENT" << endl;
    cout << circuit.pb->primary_input().size() << " "
         << commitment_pk.constraint_system.num_inputs() << endl;


    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
        r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
            commitment_pk, circuit.pb->primary_input(),
            circuit.pb->auxiliary_input());

    cout << "g_A g " << endl;
    cout << proof.g_A.g << endl;
 
auto x = proof.g_A.g.X;
auto y = proof.g_A.g.Y;
auto z = proof.g_A.g.Z;

    cout << "x " << x << endl;
    cout << "y " << y << endl;
    cout << "z " << z << endl;


// G1<default_r1cs_ppzksnark_pp> newPoint{x,y,z};
// cout << "newpoint " << endl;
// cout << newPoint << endl;

// if (newPoint == proof.g_A.g) {
//   cout << "equal!" << endl;
// } else {
//   cout << "not equal!" << endl;
// }

stringstream proofstring(ios_base::out);
proofstring << proof;
string portable = proofstring.str();


r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> newproof;
stringstream importstream(portable, ios_base::in);
importstream >> newproof;



    bool verified =
        r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
            commitment_vk, circuit.pb->primary_input(),
            newproof);

            if (proof == newproof) {
              cout << "all good" << endl;
              return 0;
            } else {
              cout << "error!" << endl;
              return 1;
            }
}