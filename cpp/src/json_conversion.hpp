#include <json/json.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

using namespace libsnark;

namespace json_conversion
{

Json::Value to_json(r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof);

}