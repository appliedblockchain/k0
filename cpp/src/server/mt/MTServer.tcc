#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <stdlib.h>
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/MTAdditionCircuit.hpp"
#include "circuitry/TransferCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "json_conversion.hpp"
#include "packing.hpp"
#include "pkutil.cpp"
#include "printbits.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"
#include "serialization.hpp"
#include "MTServer.hpp"
#include "util.h"

template <typename FieldT, typename MerkleTreeHashT>
zktrade::MTServer<FieldT, MerkleTreeHashT>::MTServer(size_t height,
                                                     AbstractServerConnector &connector,
                                                     serverVersion_t type)
    : MTStubServer(connector, type),
      tree_height{height},
      mt{height} {}

template <typename FieldT, typename MerkleTreeHashT>
string zktrade::MTServer<FieldT, MerkleTreeHashT>::reset()
{
  mt = MerkleTree<MerkleTreeHashT>{tree_height};
  return bits2hex(mt.root());
}

template <typename FieldT, typename MerkleTreeHashT>
string zktrade::MTServer<FieldT, MerkleTreeHashT>::root()
{
  return bits2hex(mt.root());
}

template <typename FieldT, typename MerkleTreeHashT>
Json::Value zktrade::MTServer<FieldT, MerkleTreeHashT>::add(
    const string &leaf_hex)
{
  cout << "leaf " << leaf_hex << endl;
  bit_vector leaf_bv = hex2bits(leaf_hex);
  cout << "mt root before" << bits2hex(mt.root()) << endl;
  cout << "mt adding " << bits2hex(leaf_bv) << endl;
  uint address = mt.add(leaf_bv);
  cout << "mt root after" << bits2hex(mt.root()) << endl;
  Json::Value result;
  result["address"] = address;
  result["newRoot"] = bits2hex(mt.root());
  return result;
}

template <typename FieldT, typename MerkleTreeHashT>
string zktrade::MTServer<FieldT, MerkleTreeHashT>::element(
    int address)
{
  if (mt.num_elements() == 0 || address > (mt.num_elements() - 1))
  {
    throw JsonRpcException(-32602, "Address too big");
  }
  return bits2hex(mt[address]);
}

template <typename FieldT, typename MerkleTreeHashT>
Json::Value zktrade::MTServer<FieldT, MerkleTreeHashT>::simulateAddition(
    const std::string &cm_str)
{
  bit_vector cm_bits = hex2bits(cm_str);
  auto sim_result = mt.simulate_add(cm_bits);
  Json::Value result;
  result["address"] = get<0>(sim_result);
  result["newRoot"] = bits2hex(get<1>(sim_result));
  vector<bit_vector> path_vec = get<2>(sim_result);

  Json::Value path;
  for (int i = 0; i < path_vec.size(); i++) {
    path[i] = bits2hex(path_vec[i]);
  }
  result["path"] = path;

  return result;
}

template <typename FieldT, typename MerkleTreeHashT>
Json::Value
zktrade::MTServer<FieldT, MerkleTreeHashT>::status()
{
  Json::Value result;
  result["ready"] = true;
  return result;
}