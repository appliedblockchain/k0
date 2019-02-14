#pragma once

#include "merkletreestubserver.h"
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "../MerkleTree.hpp"
#include "../circuitry/AdditionCircuit.hpp"
#include "../circuitry/InclusionCircuit.hpp"

using namespace jsonrpc;
using namespace libsnark;

template<typename FieldT, typename HashT>
class MerkleTreeServer : public MerkleTreeStubServer
{
  r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> addition_pk;
  r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> addition_vk;
  r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> inclusion_pk;
  r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> inclusion_vk;
  MerkleTree mt;
  AdditionCircuit<FieldT, HashT> addition_circuit;
  InclusionCircuit<FieldT, HashT> inclusion_circuit;
  size_t tree_height;
  bool addition_pk_loaded;
  bool addition_vk_loaded;
  bool inclusion_pk_loaded;
  bool inclusion_vk_loaded;
public:
  MerkleTreeServer(size_t tree_height, AbstractServerConnector &connector, serverVersion_t type);
  void setAdditionPk(std::string pk_path);
  void setAdditionVk(std::string pk_path);
  void setInclusionPk(std::string pk_path);
  void setInclusionVk(std::string pk_path);
  virtual Json::Value add(const std::string& param01);
  virtual std::string element(int param01);
  virtual std::string generateCommitment(const std::string& a_pk, const std::string& rho, const std::string& r, const std::string& v);
  virtual std::string hash(const std::string& param01, const std::string& param02);
  virtual Json::Value proveInclusion(int param01, const std::string& param02, const std::string& param03);
  virtual std::string reset();
  virtual std::string root();
  virtual Json::Value simulateAddition(const std::string& param01);
  virtual Json::Value status();
};

#include "MerkleTreeServer.tcc"