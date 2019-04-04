#include "json_conversion.hpp"

using namespace libff;
using namespace libsnark;
using namespace std;

namespace json_conversion
{


Json::Value g1_to_json_affine(G1<default_r1cs_ppzksnark_pp> input)
{
  G1<default_r1cs_ppzksnark_pp> copy(input);
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

Json::Value g2_to_json_affine(G2<default_r1cs_ppzksnark_pp> input)
{
  G2<default_r1cs_ppzksnark_pp> copy(input);
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

Json::Value proof_to_json_affine(
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof) {
  Json::Value proof_in_json;
  proof_in_json[0] = json_conversion::g1_to_json_affine(proof.g_A.g);
  proof_in_json[1] = json_conversion::g1_to_json_affine(proof.g_A.h);
  proof_in_json[2] = json_conversion::g2_to_json_affine(proof.g_B.g);
  proof_in_json[3] = json_conversion::g1_to_json_affine(proof.g_B.h);
  proof_in_json[4] = json_conversion::g1_to_json_affine(proof.g_C.g);
  proof_in_json[5] = json_conversion::g1_to_json_affine(proof.g_C.h);
  proof_in_json[6] = json_conversion::g1_to_json_affine(proof.g_H);
  proof_in_json[7] = json_conversion::g1_to_json_affine(proof.g_K);
  return proof_in_json;
}

string num_to_string(alt_bn128_Fq num)
{
  stringstream stream(ios_base::out);
  stream << num;
  return stream.str();
}

alt_bn128_Fq string_to_num(string str)
{
  bigint<alt_bn128_q_limbs> num{str.c_str()};
  return num;
}

Json::Value g1_to_json(G1<default_r1cs_ppzksnark_pp> input)
{
  Json::Value result;
  result["x"] = num_to_string(input.X);
  result["y"] = num_to_string(input.Y);
  result["z"] = num_to_string(input.Z);
  return result;
}

G1<default_r1cs_ppzksnark_pp> json_to_g1(Json::Value j)
{
  G1<default_r1cs_ppzksnark_pp> result{
      string_to_num(j["x"].asString()),
      string_to_num(j["y"].asString()),
      string_to_num(j["z"].asString())};
  return result;
}

Json::Value g2_to_json(G2<default_r1cs_ppzksnark_pp> input)
{
  Json::Value result;
  result["x"]["c0"] = num_to_string(input.X.c0);
  result["x"]["c1"] = num_to_string(input.X.c1);
  result["y"]["c0"] = num_to_string(input.Y.c0);
  result["y"]["c1"] = num_to_string(input.Y.c1);
  result["z"]["c0"] = num_to_string(input.Z.c0);
  result["z"]["c1"] = num_to_string(input.Z.c1);
  return result;
}

G2<default_r1cs_ppzksnark_pp> json_to_g2(Json::Value input)
{
  G2<default_r1cs_ppzksnark_pp> result{
      {string_to_num(input["x"]["c0"].asString()),
       string_to_num(input["x"]["c1"].asString())},
      {string_to_num(input["y"]["c0"].asString()),
       string_to_num(input["y"]["c1"].asString())},
      {string_to_num(input["z"]["c0"].asString()),
       string_to_num(input["z"]["c1"].asString())}};
  return result;
}

Json::Value proof_to_json_jacobian(
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof)
{
  Json::Value p;
  p["g_A"]["g"] = g1_to_json(proof.g_A.g);
  p["g_A"]["h"] = g1_to_json(proof.g_A.h);
  p["g_B"]["g"] = g2_to_json(proof.g_B.g);
  p["g_B"]["h"] = g1_to_json(proof.g_B.h);
  p["g_C"]["g"] = g1_to_json(proof.g_C.g);
  p["g_C"]["h"] = g1_to_json(proof.g_C.h);
  p["g_H"] = g1_to_json(proof.g_H);
  p["g_K"] = g1_to_json(proof.g_K);
  return p;
}

r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> json_to_proof_jacobian(
  Json::Value input)
{
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof{
      {json_to_g1(input["g_A"]["g"]), json_to_g1(input["g_A"]["h"])},
      {json_to_g2(input["g_B"]["g"]), json_to_g1(input["g_B"]["h"])},
      {json_to_g1(input["g_C"]["g"]), json_to_g1(input["g_C"]["h"])},
      json_to_g1(input["g_H"]),
      json_to_g1(input["g_K"])};
  return proof;
}

} // namespace json