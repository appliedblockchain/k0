#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>

using namespace libff;
using namespace libsnark;

template<typename FieldT>
zktrade::MTAdditionCircuit<FieldT>
zktrade::make_mt_addition_circuit(size_t tree_height) {

    typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;
    auto pb = new protoboard<FieldT>();

    auto prev_root_packed = new pb_variable_array<FieldT>();
    prev_root_packed->allocate(*pb, 2, "prev_root_packed");

    auto address_packed = new pb_variable<FieldT>();
    address_packed->allocate(*pb, "address_packed");

    auto next_leaf_packed = new pb_variable_array<FieldT>();
    next_leaf_packed->allocate(*pb, 2, "next_leaf_packed");

    auto next_root_packed = new pb_variable_array<FieldT>();
    next_root_packed->allocate(*pb, 2, "next_root_packed");

    pb->set_input_sizes(7);

    auto address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    auto prev_leaf_bits = new digest_variable<FieldT>(*pb, 256,
                                                      "prev_leaf_bits");
    auto prev_root_bits = new digest_variable<FieldT>(*pb, 256,
                                                      "prev_root_bits");
    auto prev_path_var =
            new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(
                    *pb, tree_height, "prev_path_var");

    auto next_leaf_bits = new digest_variable<FieldT>(*pb, 256,
                                                      "next_leaf_bits");
    auto next_root_bits = new digest_variable<FieldT>(*pb, 256,
                                                      "next_root_bits");
    auto next_path_var =
            new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(
                    *pb, tree_height, "next_path_var");

    auto prev_root_packer =
            new multipacking_gadget<FieldT>(*pb, prev_root_bits->bits,
                                            *prev_root_packed, 128,
                                            "prev_root_packer");
    auto address_packer =
            new packing_gadget<FieldT>(*pb, *address_bits, *address_packed,
                                       "address_packer");

    auto next_leaf_packer =
            new multipacking_gadget<FieldT>(*pb, next_leaf_bits->bits,
                                            *next_leaf_packed, 128,
                                            "next_leaf_packer");

    auto next_root_packer =
            new multipacking_gadget<FieldT>(*pb, next_root_bits->bits,
                                            *next_root_packed, 128,
                                            "next_root_packer");


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
    prev_root_packer->generate_r1cs_constraints(true);
    address_packer->generate_r1cs_constraints(true);
    next_leaf_packer->generate_r1cs_constraints(true);
    next_root_packer->generate_r1cs_constraints(true);

    mt_update_gadget->generate_r1cs_constraints();

    MTAdditionCircuit <FieldT> circuit{
            pb,
            prev_root_packed,
            address_packed,
            next_leaf_packed,
            next_root_packed,
            address_bits,
            prev_leaf_bits,
            prev_root_bits,
            prev_path_var,
            next_leaf_bits,
            next_root_bits,
            next_path_var,
            prev_root_packer,
            address_packer,
            next_leaf_packer,
            next_root_packer,
            mt_update_gadget};

    return circuit;
}