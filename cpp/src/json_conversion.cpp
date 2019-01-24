#include "json_conversion.hpp"

using namespace libsnark;

namespace json_conversion
{


Json::Value g1(libff::G1<libsnark::default_r1cs_ppzksnark_pp> input)
{
  libff::G1<libsnark::default_r1cs_ppzksnark_pp> copy(input);
  copy.to_affine_coordinates();

  std::stringstream x_stream;
  x_stream << copy.X;

  std::stringstream y_stream;
  y_stream << copy.Y;

  Json::Value result;
  result[0] = x_stream.str();
  result[1] = y_stream.str();
  return result;
}

Json::Value g2(libff::G2<libsnark::default_r1cs_ppzksnark_pp> input)
{
  libff::G2<libsnark::default_r1cs_ppzksnark_pp> copy(input);
  copy.to_affine_coordinates();

  Json::Value x;
  std::stringstream x_c1_stream;
  x_c1_stream << copy.X.c1;
  x[0] = x_c1_stream.str();
  std::stringstream x_c2_stream;
  x_c2_stream << copy.X.c0;
  x[1] = x_c2_stream.str();

  Json::Value y;
  std::stringstream y_c1_stream;
  y_c1_stream << copy.Y.c1;
  y[0] = y_c1_stream.str();
  std::stringstream y_c2_stream;
  y_c2_stream << copy.Y.c0;
  y[1] = y_c2_stream.str();

  Json::Value result;
  result[0] = x;
  result[1] = y;
  return result;
}

Json::Value to_json(r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof) {
  Json::Value proof_in_json;
  proof_in_json[0] = json_conversion::g1(proof.g_A.g);
  proof_in_json[1] = json_conversion::g1(proof.g_A.h);
  proof_in_json[2] = json_conversion::g2(proof.g_B.g);
  proof_in_json[3] = json_conversion::g1(proof.g_B.h);
  proof_in_json[4] = json_conversion::g1(proof.g_C.g);
  proof_in_json[5] = json_conversion::g1(proof.g_C.h);
  proof_in_json[6] = json_conversion::g1(proof.g_H);
  proof_in_json[7] = json_conversion::g1(proof.g_K);
  return proof_in_json;
}
 
} // namespace json