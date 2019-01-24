#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "MTLeafAdditionPacked.hpp"

using namespace libff;
using namespace libsnark;

template<typename FieldT, typename HashT>
struct AdditionCircuit
{
    protoboard<FieldT>* pb;
    pb_variable<FieldT>* address_v;
    pb_variable_array<FieldT>* prev_root_va;
    pb_variable_array<FieldT>* leaf_va;
    pb_variable_array<FieldT>* next_root_va;
    merkle_authentication_path_variable<FieldT, HashT>* path_var;
    pb_variable_array<FieldT>* address_bits_va;
    packing_gadget<FieldT>* address_unpacker;
    MTLeafAdditionPacked<FieldT, HashT>* mtlap;
};

template<typename FieldT, typename HashT>
AdditionCircuit<FieldT, HashT> make_addition_circuit(size_t tree_height);
 
#include "AdditionCircuit.tcc"