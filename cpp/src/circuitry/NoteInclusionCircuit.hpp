#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "../depends/zcash/src/zcash/circuit/commitment.tcc"
#include "../depends/zcash/src/zcash/circuit/prfs.tcc"

using namespace std;
using namespace libff;
using namespace libsnark;

template<typename FieldT, typename HashT>
struct NoteInclusionCircuit
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
    shared_ptr<digest_variable<FieldT>> commitment_bits;
    
    // Unpackers
    multipacking_gadget<FieldT>* root_packer;
    packing_gadget<FieldT>* v_packer;
    
    // Workhorse gadgets
    note_commitment_gadget<FieldT>* ncg;
};

template<typename FieldT, typename HashT>
NoteInclusionCircuit<FieldT, HashT> make_note_inclusion_circuit(size_t tree_height);

template<typename FieldT, typename HashT>
NoteInclusionCircuit<FieldT, HashT> make_note_inclusion_circuit(size_t tree_height)
{
    protoboard<FieldT>* pb = new protoboard<FieldT>();
    
    pb_variable_array<FieldT>* root_packed = new pb_variable_array<FieldT>();
    root_packed->allocate(*pb, 2, "root_packed");
    
    pb_variable<FieldT>* v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");
    
    pb->set_input_sizes(4);
    
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
    r_bits->allocate(*pb, 256, "r_bits");

    digest_variable<FieldT> commitment_bits_var(*pb, 256, "commitment_bits");
    std::shared_ptr<digest_variable<FieldT>> commitment_bits = std::make_shared<digest_variable<FieldT>>(commitment_bits_var);
    
    multipacking_gadget<FieldT>* root_packer = new multipacking_gadget<FieldT>(*pb, root_bits->bits, *root_packed, 128, "root_packer");
    packing_gadget<FieldT>* v_packer = new packing_gadget<FieldT>(*pb, *v_bits, *v_packed, "v_unpacker");

    note_commitment_gadget<FieldT>* ncg = new note_commitment_gadget<FieldT>(*pb, *ZERO, *a_pk_bits, *v_bits, *rho_bits, *r_bits, commitment_bits);
    
    pb->add_r1cs_constraint(r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()));
    
    commitment_bits_var.generate_r1cs_constraints();
    root_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    ncg->generate_r1cs_constraints();
    
    NoteInclusionCircuit<FieldT, HashT> circuit{
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
        root_packer,
        v_packer,
        ncg
    };
    
    return circuit;
}
