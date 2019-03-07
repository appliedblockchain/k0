#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "file_output.hpp"
#include "pkutil.cpp"

using namespace libsnark;
using namespace std;

typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "Need exactly 2 arguments (path to source vk, path to target vk)" << endl;
    return 1;
  }
  default_r1cs_ppzksnark_pp::init_public_params();

  r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(argv[1]);
  print_vk_to_file<default_r1cs_ppzksnark_pp>(vk, argv[2]);
}