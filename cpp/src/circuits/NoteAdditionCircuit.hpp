#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>
#include "../depends/zcash/src/zcash/circuit/commitment.tcc"

using namespace std;
using namespace libff;
using namespace libsnark;

template<typename FieldT, typename HashT>
struct NoteAdditionCircuit
{
    protoboard<FieldT>* pb;

    // Primary inputs
    pb_variable<FieldT>* address_packed;
    pb_variable_array<FieldT>* prev_root_packed;
    pb_variable<FieldT>* v_packed;
    pb_variable_array<FieldT>* commitment_packed;
    pb_variable_array<FieldT>* next_root_packed;

    // Provided auxiliary inputs
    digest_variable<FieldT>* prev_root_bits;
    pb_variable_array<FieldT>* a_pk_bits;
    pb_variable_array<FieldT>* rho_bits;
    pb_variable_array<FieldT>* r_bits;
    merkle_authentication_path_variable<FieldT, HashT>* prev_path;
    merkle_authentication_path_variable<FieldT, HashT>* next_path;

    // Generated auxiliary inputs
    pb_variable_array<FieldT>* address_bits;
    pb_variable_array<FieldT>* v_bits;
    shared_ptr<digest_variable<FieldT>> commitment_bits;
    digest_variable<FieldT>* next_root_bits;

    // Unpackers
    packing_gadget<FieldT>* address_unpacker;
    multipacking_gadget<FieldT>* prev_root_unpacker;
    packing_gadget<FieldT>* v_unpacker;
    multipacking_gadget<FieldT>* commitment_unpacker;
    multipacking_gadget<FieldT>* next_root_unpacker;

    // "Workhorse" gadgets
    note_commitment_gadget<FieldT>* ncg;
    merkle_tree_check_update_gadget<FieldT, HashT>* mtcug;
};

template<typename FieldT, typename HashT>
NoteAdditionCircuit<FieldT, HashT> make_note_addition_circuit(size_t tree_height);

template<typename FieldT, typename HashT>
NoteAdditionCircuit<FieldT, HashT> make_note_addition_circuit(size_t tree_height) {
    protoboard<FieldT>* pb = new protoboard<FieldT>();
    

    pb_variable<FieldT>* address_packed = new pb_variable<FieldT>();
    address_packed->allocate(*pb, "address_packed");

    pb_variable_array<FieldT>* prev_root_packed = new pb_variable_array<FieldT>();
    prev_root_packed->allocate(*pb, 2, "prev_root_packed");

    pb_variable<FieldT>* v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");

    pb_variable_array<FieldT>* commitment_packed = new pb_variable_array<FieldT>();
    commitment_packed->allocate(*pb, 2, "commitment_packed");
    
    pb_variable_array<FieldT>* next_root_packed = new pb_variable_array<FieldT>();
    next_root_packed->allocate(*pb, 2, "next_root_packed");

    pb->set_input_sizes(8);

    pb_variable<FieldT>* ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb);
    
    pb_variable_array<FieldT>* a_pk_bits = new pb_variable_array<FieldT>();
    a_pk_bits->allocate(*pb, 256, "a_pk_bits");

    pb_variable_array<FieldT>* rho_bits = new pb_variable_array<FieldT>();
    rho_bits->allocate(*pb, 256, "rho_bits");

    pb_variable_array<FieldT>* r_bits = new pb_variable_array<FieldT>();
    r_bits->allocate(*pb, 256, "r_bits");

    merkle_authentication_path_variable<FieldT, HashT>* prev_path = new merkle_authentication_path_variable<FieldT, HashT>(*pb, tree_height, "prev_path");
    
    merkle_authentication_path_variable<FieldT, HashT>* next_path = new merkle_authentication_path_variable<FieldT, HashT>(*pb, tree_height, "next_path");

    digest_variable<FieldT>* prev_root_bits = new digest_variable<FieldT>(*pb, 256, "prev_root_bits");
 
    pb_variable_array<FieldT>* address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    pb_variable_array<FieldT>* v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");

    digest_variable<FieldT>* prev_leaf_bits = new digest_variable<FieldT>(*pb, 256, "prev_leaf_bits");
 
    digest_variable<FieldT> commitment_bits_var(*pb, 256, "commitment_bits");
    std::shared_ptr<digest_variable<FieldT>> commitment_bits = std::make_shared<digest_variable<FieldT>>(commitment_bits_var);
 
    digest_variable<FieldT>* next_root_bits = new digest_variable<FieldT>(*pb, 256, "next_root_bits");
   
    packing_gadget<FieldT>* address_unpacker = new packing_gadget<FieldT>(*pb, *address_bits, *address_packed, "address_unpacker");
    
    multipacking_gadget<FieldT>* prev_root_unpacker = new multipacking_gadget<FieldT>(*pb, prev_root_bits->bits, *prev_root_packed, 128, "prev_root_unpacker");
    packing_gadget<FieldT>* v_unpacker = new packing_gadget<FieldT>(*pb, *v_bits, *v_packed, "v_unpacker");

    multipacking_gadget<FieldT>* commitment_unpacker = new multipacking_gadget<FieldT>(*pb, commitment_bits_var.bits, *commitment_packed, 128, "commitment_unpacker");
    
    multipacking_gadget<FieldT>* next_root_unpacker = new multipacking_gadget<FieldT>(*pb, next_root_bits->bits, *next_root_packed, 128, "next_root_unpacker");
    
    note_commitment_gadget<FieldT>* ncg = new note_commitment_gadget<FieldT>(*pb, *ZERO, *a_pk_bits, *v_bits, *rho_bits, *r_bits, commitment_bits);

    merkle_tree_check_update_gadget<FieldT, HashT>* mtcug = new merkle_tree_check_update_gadget<FieldT, HashT>(
        *pb,
        tree_height,
        *address_bits,
        *prev_leaf_bits,
        *prev_root_bits,
        *prev_path,
        commitment_bits_var,
        *next_root_bits,
        *next_path,
        ONE,
        "mtug");
    
    pb->add_r1cs_constraint(r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()));
    
    prev_path->generate_r1cs_constraints();
    next_path->generate_r1cs_constraints();

    address_unpacker->generate_r1cs_constraints(true);
    prev_root_unpacker->generate_r1cs_constraints(true);
    v_unpacker->generate_r1cs_constraints(true);
    commitment_unpacker->generate_r1cs_constraints(true);
    next_root_unpacker->generate_r1cs_constraints(true);

    // We're only adding elements, therefore the previous leaf always needs to be zero
    for (int i = 0; i < 256; i++) {
        
        pb->add_r1cs_constraint(r1cs_constraint<FieldT>(prev_leaf_bits->bits[i], ONE, *ZERO));
    }

    ncg->generate_r1cs_constraints();
    mtcug->generate_r1cs_constraints();

    NoteAdditionCircuit<FieldT, HashT> circuit{
        pb,
        address_packed,
        prev_root_packed,
        v_packed,
        commitment_packed,
        next_root_packed,
        prev_root_bits,
        a_pk_bits,
        rho_bits,
        r_bits,
        prev_path,
        next_path,
        address_bits,
        v_bits,
        commitment_bits,
        next_root_bits,
        address_unpacker,
        prev_root_unpacker,
        v_unpacker,
        commitment_unpacker,
        next_root_unpacker,
        ncg,
        mtcug
    };

   return circuit;
}
