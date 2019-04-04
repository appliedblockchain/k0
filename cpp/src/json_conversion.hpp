#include <json/json.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

using namespace libsnark;

namespace json_conversion
{

Json::Value proof_to_json_jacobian(
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof);

r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> json_to_proof_jacobian(
  Json::Value input);

Json::Value proof_to_json_affine(
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof);

}