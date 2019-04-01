#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

using namespace std;
using namespace libsnark;

string proof_to_string(r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof);

r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> string_to_proof(string str);

