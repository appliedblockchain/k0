#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "./pkutil.cpp"

using namespace libsnark;
using namespace std;

typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

bool verify(
  string vk_path,
  string proof_path,
  r1cs_ppzksnark_primary_input<default_r1cs_ppzksnark_pp> primary_input
  ) {
  r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(vk_path);
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof_from_file = loadFromFile<r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp>>(proof_path);
  bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(vk, primary_input, proof_from_file);
  if (verified) {
    cout << "Successfully verified." << endl;
  } else {
    cerr << "Verification failed." << endl;
  }

  return verified ? 0 : 255;
}