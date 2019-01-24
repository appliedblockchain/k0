#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "pkutil.cpp"

using namespace libsnark;
using namespace std;

typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

void setup(r1cs_constraint_system<FieldT> constraint_system, string pk_path, string vk_path) {
  const r1cs_ppzksnark_keypair<default_r1cs_ppzksnark_pp> keypair = r1cs_ppzksnark_generator<default_r1cs_ppzksnark_pp>(constraint_system);
  cout << "Writing proving key to " << pk_path << endl;
  writeToFile(pk_path, keypair.pk);
  cout << "Writing verification key to " << vk_path << endl;
  writeToFile(vk_path, keypair.vk);
}
