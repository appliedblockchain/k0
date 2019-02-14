#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>
#include "MerkleTreeServer.hpp"
#include "../pkutil.cpp"
#include "../serialization.hpp"
#include "../printbits.hpp"
#include "../json_conversion.hpp"
#include "../packing.hpp"
#include "../util.h"
#include "../scheme/cm.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_two_to_one_hash_gadget<FieldT> HashT;

const size_t digest_len = HashT::get_digest_len();

template <typename FieldT, typename HashT>
MerkleTreeServer<FieldT, HashT>::MerkleTreeServer(size_t height, AbstractServerConnector &connector, serverVersion_t type) : MerkleTreeStubServer(connector, type),
                                                                                                                             tree_height{height},
                                                                                                                             mt{height},
                                                                                                                             addition_circuit{make_addition_circuit<FieldT, HashT>(height)},
                                                                                                                             inclusion_circuit{make_inclusion_circuit<FieldT, HashT>(height)}
{
}

template <typename FieldT, typename HashT>
string MerkleTreeServer<FieldT, HashT>::reset()
{
  mt = MerkleTree{tree_height};
  return bits2hex(mt.root());
}

template <typename FieldT, typename HashT>
string MerkleTreeServer<FieldT, HashT>::root()
{
  return bits2hex(mt.root());
}

template <typename FieldT, typename HashT>
void MerkleTreeServer<FieldT, HashT>::setAdditionPk(string pk_path)
{
  addition_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(pk_path);
  addition_pk_loaded = true;
}

template <typename FieldT, typename HashT>
void MerkleTreeServer<FieldT, HashT>::setAdditionVk(string vk_path)
{
  addition_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(vk_path);
  addition_vk_loaded = true;
}

template <typename FieldT, typename HashT>
void MerkleTreeServer<FieldT, HashT>::setInclusionPk(string pk_path)
{
  inclusion_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(pk_path);
  inclusion_pk_loaded = true;
}

template <typename FieldT, typename HashT>
void MerkleTreeServer<FieldT, HashT>::setInclusionVk(string vk_path)
{
  inclusion_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(vk_path);
  inclusion_vk_loaded = true;
}

template <typename FieldT, typename HashT>
std::string MerkleTreeServer<FieldT, HashT>::generateCommitment(const std::string& a_pk_str, const std::string& rho_str, const std::string& r_str, const std::string& v_str)
{
  bit_vector a_pk = hex2bits(a_pk_str);
  bit_vector rho = hex2bits(rho_str);
  bit_vector r = hex2bits(r_str);
  bit_vector v = hex2bits(v_str);
  return bits_to_hex(cm(a_pk, rho, r, v));
}

template <typename FieldT, typename HashT>
Json::Value MerkleTreeServer<FieldT, HashT>::simulateAddition(const string &leaf_hex)
{
  if (leaf_hex == "")
  {
    throw JsonRpcException(-32100, "Input was empty");
  }

  bit_vector leaf_bv = hex2bits(leaf_hex);
  auto simulation_result = mt.simulate_add(leaf_bv);
  uint address = get<0>(simulation_result);
  bit_vector next_root_bv = get<1>(simulation_result);
  vector<bit_vector> path = get<2>(simulation_result);

  addition_circuit.pb->val(*addition_circuit.address_v) = FieldT(address);
  addition_circuit.address_unpacker->generate_r1cs_witness_from_packed();

  auto prev_root_bv = mt.root();
  auto prev_root_field_elems = field_elements_from_bits<FieldT>(prev_root_bv);
  addition_circuit.prev_root_va->fill_with_field_elements(*addition_circuit.pb, prev_root_field_elems);

  vector<FieldT> leaf_field_elems = pack<FieldT>(leaf_bv);
  addition_circuit.leaf_va->fill_with_field_elements(*addition_circuit.pb, leaf_field_elems);

  auto next_root_elems = field_elements_from_bits<FieldT>(next_root_bv);
  addition_circuit.next_root_va->fill_with_field_elements(*addition_circuit.pb, next_root_elems);

  addition_circuit.path_var->generate_r1cs_witness(address, path);

  cout << addition_circuit.pb->primary_input() << endl;

  if (addition_circuit.pb->is_satisfied())
  {
    cout << "SATISFIED 1" << endl;
  }
  else
  {
    cout << "NOT SATISFIED 1 (as expected)" << endl;
  }
  addition_circuit.mtlap->generate_r1cs_witness();
  if (addition_circuit.pb->is_satisfied())
  {
    cout << "SATISFIED 2 (as expected)" << endl;
  }
  else
  {
    cout << "NOT SATISFIED 2" << endl;
  }
  const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof = r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(addition_pk, addition_circuit.pb->primary_input(), addition_circuit.pb->auxiliary_input());
  bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(addition_vk, addition_circuit.pb->primary_input(), proof);
  if (verified)
  {
    cout << "Successfully verified." << endl;
  }
  else
  {
    cerr << "Verification failed." << endl;
  }

  writeToFile("/tmp/serverproof", proof);
  Json::Value proof_in_json = json_conversion::to_json(proof);

  auto new_root_elems = field_elements_from_bits<FieldT>(next_root_bv);
  Json::Value new_root_json;

  stringstream stream_0;
  stream_0 << new_root_elems[0];
  new_root_json[0] = stream_0.str();

  stringstream stream_1;
  stream_1 << new_root_elems[1];
  new_root_json[1] = stream_1.str();

  //vector<string> primary_input_strings;
  //std::transform(addition_circuit.pb->primary_input().begin(), addition_circuit.pb->primary_input().end(), primary_input_strings.begin(), field_element_to_string);
  Json::Value result;
  // Json::Value primary_input_in_json;
  // for (uint i = 0; i < addition_circuit.pb->primary_input().size(); i ++) {
  //   primary_input_in_json[i] = field_element_to_string(addition_circuit.pb->primary_input()[i]);
  // }
  result["address"] = address;
  result["newRoot"] = new_root_json;
  result["proof"] = proof_in_json;
  return result;
}

template <typename FieldT, typename HashT>
Json::Value MerkleTreeServer<FieldT, HashT>::add(const string &leaf_hex)
{
  bit_vector leaf_bv = hex2bits(leaf_hex);
  uint address = mt.add(leaf_bv);
  mt.print();
  Json::Value result;
  result["address"] = address;
  result["newRoot"] = bits2hex(mt.root());
  return result;
}

template <typename FieldT, typename HashT>
string MerkleTreeServer<FieldT, HashT>::element(int address)
{
  if (mt.num_elements() == 0 || address > (mt.num_elements() - 1))
  {
    throw JsonRpcException(-32602, "Address too big");
  }
  return bits2hex(mt[address]);
}

template <typename FieldT, typename HashT>
std::string MerkleTreeServer<FieldT, HashT>::hash(const std::string &left_hex,
                                                  const std::string &right_hex)
{
  libff::bit_vector block = hex2bits(left_hex);
  libff::bit_vector right = hex2bits(right_hex);
  block.insert(block.end(), right.begin(), right.end());
  return bits2hex(HashT::get_hash(block));
}

template <typename FieldT, typename HashT>
Json::Value MerkleTreeServer<FieldT, HashT>::proveInclusion(int address, const string &r_hex, const string &sn_hex)
{
  if (mt.num_elements() == 0 || address > (mt.num_elements() - 1))
  {
    throw JsonRpcException(-32602, "Address too big");
  }

  libff::bit_vector r_bv = hex2bits(r_hex);
  libff::bit_vector sn_bv = hex2bits(sn_hex);

  libff::bit_vector block = r_bv;
  block.insert(block.end(), sn_bv.begin(), sn_bv.end());
  libff::bit_vector leaf_bv = HashT::get_hash(block);
  assert(leaf_bv == mt[address]);

  const libff::bit_vector address_bits = libff::convert_field_element_to_bit_vector<FieldT>(FieldT(address, true), tree_height);

  auto root_bv = mt.root();
  auto root_field_elems = field_elements_from_bits<FieldT>(root_bv);
  inclusion_circuit.root_va->fill_with_field_elements(*inclusion_circuit.pb, root_field_elems);

  auto sn_field_elems = field_elements_from_bits<FieldT>(sn_bv);
  inclusion_circuit.sn_va->fill_with_field_elements(*inclusion_circuit.pb, sn_field_elems);

  inclusion_circuit.root_unpacker->generate_r1cs_witness_from_packed();
  inclusion_circuit.sn_unpacker->generate_r1cs_witness_from_packed();
  inclusion_circuit.r_digest->generate_r1cs_witness(r_bv);
  inclusion_circuit.leaf_hasher->generate_r1cs_witness();

  inclusion_circuit.address_bits_va->fill_with_bits(*inclusion_circuit.pb, address_bits);
  inclusion_circuit.path_var->generate_r1cs_witness(address, mt.path(address));

  inclusion_circuit.mtcrg->generate_r1cs_witness();

  cout << "Root" << endl;
  cout << bits2hex(root_bv) << endl;
  cout << bits2hex(inclusion_circuit.root_digest->get_digest()) << endl;
  cout << endl;
 
  cout << "Leaf" << endl;
  cout << bits2hex(leaf_bv) << endl;
  cout << bits2hex(inclusion_circuit.leaf_digest->get_digest()) << endl;
  cout << endl;

  if (inclusion_circuit.pb->is_satisfied())
  {
    cout << "SATISFIED" << endl;
  }
  else
  {
    cout << "NOT SATISFIED" << endl;
  }

  const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof = r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(inclusion_pk, inclusion_circuit.pb->primary_input(), inclusion_circuit.pb->auxiliary_input());
  bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(inclusion_vk, inclusion_circuit.pb->primary_input(), proof);
  if (verified)
  {
    cout << "Successfully verified." << endl;
  }
  else
  {
    cerr << "Verification failed." << endl;
  }

  return json_conversion::to_json(proof);
}

template <typename FieldT, typename HashT>
Json::Value MerkleTreeServer<FieldT, HashT>::status()
{
  const bool ready = addition_pk_loaded & addition_vk_loaded & inclusion_pk_loaded & inclusion_vk_loaded;
  Json::Value result;
  result["ready"] = ready;
  return result;
}