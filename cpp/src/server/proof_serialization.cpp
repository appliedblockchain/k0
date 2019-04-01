#include "proof_serialization.hpp"

string proof_to_string(r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof)
{
  stringstream proofstring(ios_base::out);
  proofstring << proof;
  return proofstring.str();
}

r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> string_to_proof(string str)
{
  r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> newproof;
  stringstream importstream(str, ios_base::in);
  importstream >> newproof;
  return newproof;
}