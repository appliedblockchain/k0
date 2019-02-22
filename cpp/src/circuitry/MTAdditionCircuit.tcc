#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>

using namespace libff;
using namespace libsnark;

template<typename FieldT>
zktrade::MTAdditionCircuit<FieldT> zktrade::make_mt_addition_circuit(size_t tree_height) {

    typedef sha256_two_to_one_hash_gadget <FieldT> TwoToOneSHA256;
    auto pb = new protoboard<FieldT>();
    auto address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    auto prev_leaf_bits = new digest_variable<FieldT>(*pb, 256, "prev_leaf_bits");
    auto prev_root_bits = new digest_variable<FieldT>(*pb, 256, "prev_root_bits");
    auto prev_path_var =
            new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(
                    *pb, tree_height, "prev_path_var");

    auto next_leaf_bits = new digest_variable<FieldT>(*pb, 256, "next_leaf_bits");
    auto next_root_bits = new digest_variable<FieldT>(*pb, 256, "next_root_bits");
    auto next_path_var =
            new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(
                    *pb, tree_height, "next_path_var");

    auto mt_update_gadget = new merkle_tree_check_update_gadget<FieldT, TwoToOneSHA256>(
            *pb,
            tree_height,
            *address_bits,
            *prev_leaf_bits,
            *prev_root_bits,
            *prev_path_var,
            *next_leaf_bits,
            *next_root_bits,
            *next_path_var,
            ONE,
            "mt_update_gadget");

    prev_leaf_bits->generate_r1cs_constraints();
    prev_root_bits->generate_r1cs_constraints();
    prev_path_var->generate_r1cs_constraints();
    next_leaf_bits->generate_r1cs_constraints();
    next_root_bits->generate_r1cs_constraints();
    next_path_var->generate_r1cs_constraints();
    mt_update_gadget->generate_r1cs_constraints();

    MTAdditionCircuit<FieldT> circuit{
            pb,
            address_bits,
            prev_leaf_bits,
            prev_root_bits,
            prev_path_var,
            next_leaf_bits,
            next_root_bits,
            next_path_var,
            mt_update_gadget};
    return circuit;
}