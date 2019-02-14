#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "../util.h"

using namespace libff;
using namespace libsnark;

template<typename FieldT, typename HashT>
AdditionCircuit<FieldT, HashT> make_addition_circuit(size_t tree_height) {
    protoboard<FieldT>* pb = new protoboard<FieldT>();

    pb_variable<FieldT>* address_v = new pb_variable<FieldT>();
    address_v->allocate(*pb, "address");

    pb_variable_array<FieldT>* prev_root_va = new pb_variable_array<FieldT>();
    prev_root_va->allocate(*pb, 2, "prev_root");

    pb_variable_array<FieldT>* leaf_va = new pb_variable_array<FieldT>();
    leaf_va->allocate(*pb, 2, "leaf");

    pb_variable_array<FieldT>* next_root_va = new pb_variable_array<FieldT>();
    next_root_va->allocate(*pb, 2, "next_root");

    pb_variable_array<FieldT>* address_bits_va = new pb_variable_array<FieldT>();
    address_bits_va->allocate(*pb, tree_height, "address_bits");

    merkle_authentication_path_variable<FieldT, HashT>* path_var = new merkle_authentication_path_variable<FieldT, HashT>(*pb, tree_height, "merkle_authentication_path");

    packing_gadget<FieldT>* address_unpacker = new packing_gadget<FieldT>(*pb, *address_bits_va, *address_v, "address_unpacker");

    MTLeafAdditionPacked<FieldT, HashT>* mtlap = new MTLeafAdditionPacked<FieldT, HashT>(*pb,
                                          tree_height,
                                          *address_bits_va,
                                          *prev_root_va,
                                          *leaf_va,
                                          *next_root_va,
                                          *path_var);
    pb->set_input_sizes(7);
    address_unpacker->generate_r1cs_constraints(true);
    // TODO generate constraints for digests, path_var etc.?!
    mtlap->generate_r1cs_constraints();

    AdditionCircuit<FieldT, HashT> circuit{ pb, address_v, prev_root_va, leaf_va, next_root_va, path_var, address_bits_va, address_unpacker, mtlap};
    return circuit;
}
