#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>

using namespace libff;
using namespace libsnark;

template<typename FieldT, typename HashT>
struct InclusionCircuit
{
  protoboard<FieldT>* pb;
  pb_variable_array<FieldT>* root_va;
  digest_variable<FieldT>* root_digest;
  multipacking_gadget<FieldT>* root_unpacker;
  pb_variable_array<FieldT>* address_bits_va;
  digest_variable<FieldT>* leaf_digest;
  merkle_authentication_path_variable<FieldT, HashT>* path_var;
  merkle_tree_check_read_gadget<FieldT, HashT>* mtcrg;
};

template<typename FieldT, typename HashT>
InclusionCircuit<FieldT, HashT> make_inclusion_circuit(size_t tree_height);

#include "InclusionCircuit.tcc"