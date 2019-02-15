#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include "gadgets/cm.h"
#include "util.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

template<typename FieldT>
struct WithdrawalCircuit
{
    protoboard<FieldT>* pb;

    // Primary inputs
    pb_variable_array<FieldT>* root_packed;
    pb_variable<FieldT>* v_packed;
    
    // Auxiliary inputs
    digest_variable<FieldT>* root_bits;
    pb_variable_array<FieldT>* address_bits;
    pb_variable_array<FieldT>* a_pk_bits;
    pb_variable_array<FieldT>* v_bits;
    pb_variable_array<FieldT>* rho_bits;
    pb_variable_array<FieldT>* r_bits;
    digest_variable<FieldT>* commitment_bits;
    merkle_authentication_path_variable<FieldT, sha256_two_to_one_hash_gadget<FieldT>>* path;

    // Unpackers
    multipacking_gadget<FieldT>* root_packer;
    packing_gadget<FieldT>* v_packer;
    
    // Workhorse gadgets
    cm_gadget<FieldT>* cmg;
    merkle_tree_check_read_gadget<FieldT, sha256_two_to_one_hash_gadget<FieldT>>* mtcrg;
};

template<typename FieldT>
WithdrawalCircuit<FieldT> make_withdrawal_circuit(size_t tree_height);

template<typename FieldT>
WithdrawalCircuit<FieldT> make_withdrawal_circuit(size_t tree_height)
{
    protoboard<FieldT>* pb = new protoboard<FieldT>();
    
    pb_variable_array<FieldT>* root_packed = new pb_variable_array<FieldT>();
    root_packed->allocate(*pb, 2, "root_packed");
    
    pb_variable<FieldT>* v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");

    // TODO add: sn

    pb->set_input_sizes(3);

    // TODO add: a_sk

    pb_variable<FieldT>* ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb);
    
    digest_variable<FieldT>* root_bits = new digest_variable<FieldT>(*pb, 256, "root_bits");
    
    pb_variable_array<FieldT>* address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");
    
    pb_variable_array<FieldT>* a_pk_bits = new pb_variable_array<FieldT>();
    a_pk_bits->allocate(*pb, 256, "a_pk_bits");
    
    pb_variable_array<FieldT>* v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");
    
    pb_variable_array<FieldT>* rho_bits = new pb_variable_array<FieldT>();
    rho_bits->allocate(*pb, 256, "rho_bits");
    
    pb_variable_array<FieldT>* r_bits = new pb_variable_array<FieldT>();
    r_bits->allocate(*pb, 384, "r_bits");

    digest_variable<FieldT>* commitment_bits = new digest_variable<FieldT>(*pb, 256, "commitment_bits");

    multipacking_gadget<FieldT>* root_packer = new multipacking_gadget<FieldT>(*pb, root_bits->bits, *root_packed, 128, "root_packer");
    packing_gadget<FieldT>* v_packer = new packing_gadget<FieldT>(*pb, *v_bits, *v_packed, "v_unpacker");

    cm_gadget<FieldT>* cmg = new cm_gadget<FieldT>(*pb, *ZERO, *a_pk_bits, *rho_bits, *r_bits, *v_bits, *commitment_bits);

    typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;
    merkle_authentication_path_variable<FieldT, TwoToOneSHA256>* path = new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(*pb, tree_height, "merkle_authentication_path");
    merkle_tree_check_read_gadget<FieldT, TwoToOneSHA256>* mtcrg = new merkle_tree_check_read_gadget<FieldT, TwoToOneSHA256>(
      *pb,
      tree_height,
      *address_bits,
      *commitment_bits,
      *root_bits,
      *path,
      ONE,
      "merkle_tree_check_read_gadget");



    pb->add_r1cs_constraint(r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()));
    // TODO Merkle tree path verification
    commitment_bits->generate_r1cs_constraints();
    root_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    cmg->generate_r1cs_constraints();
    mtcrg->generate_r1cs_constraints();
    
    WithdrawalCircuit<FieldT> circuit{
        pb,
        root_packed,
        v_packed,
        root_bits,
        address_bits,
        a_pk_bits,
        v_bits,
        rho_bits,
        r_bits,
        commitment_bits,
        path,
        root_packer,
        v_packer,
        cmg,
        mtcrg
    };
    
    return circuit;
}
