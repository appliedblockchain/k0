#ifndef ZKTRADE_NEWTRANSFERCIRCUIT_HPP
#define ZKTRADE_NEWTRANSFERCIRCUIT_HPP


#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "circuitry/gadgets/input_note_gadget.hpp"
#include "circuitry/gadgets/comms.hpp"

namespace zktrade {
    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    struct NewTransferCircuit {
        protoboard<FieldT> *pb;

        // Primary inputs
        pb_variable_array<FieldT> *rt_packed;
        pb_variable_array<FieldT> *in_0_sn_packed;
        pb_variable_array<FieldT> *in_1_sn_packed;
        pb_variable_array<FieldT> *out_0_cm_packed;
        pb_variable_array<FieldT> *out_1_cm_packed;

        // TODO packed cms

        // Auxiliary inputs
        pb_variable<FieldT>* ZERO;
        digest_variable<FieldT> *rt_bits;

        // TODO use arrays/vectors
        // First input
        pb_variable_array<FieldT> *in_0_v_bits;
        pb_variable_array<FieldT> *in_0_a_sk_bits;
        pb_variable_array<FieldT> *in_0_rho_bits;
        pb_variable_array<FieldT> *in_0_r_bits;
        pb_variable_array<FieldT> *in_0_address_bits;
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_0_path;
        shared_ptr<digest_variable<FieldT>> in_0_a_pk_bits;
        digest_variable<FieldT> *in_0_cm_bits;
        shared_ptr<digest_variable<FieldT>> in_0_sn_bits;

        // Second input
        pb_variable_array<FieldT> *in_1_v_bits;
        pb_variable_array<FieldT> *in_1_a_sk_bits;
        pb_variable_array<FieldT> *in_1_rho_bits;
        pb_variable_array<FieldT> *in_1_r_bits;
        pb_variable_array<FieldT> *in_1_address_bits;
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_1_path;
        shared_ptr<digest_variable<FieldT>> in_1_a_pk_bits;
        digest_variable<FieldT> *in_1_cm_bits;
        shared_ptr<digest_variable<FieldT>> in_1_sn_bits;

        // First output
        pb_variable_array<FieldT> *out_0_v_bits;
        pb_variable_array<FieldT> *out_0_a_pk_bits;
        pb_variable_array<FieldT> *out_0_rho_bits;
        pb_variable_array<FieldT> *out_0_r_bits;
        digest_variable<FieldT> *out_0_cm_bits;

        // Second output
        pb_variable_array<FieldT> *out_1_v_bits;
        pb_variable_array<FieldT> *out_1_a_pk_bits;
        pb_variable_array<FieldT> *out_1_rho_bits;
        pb_variable_array<FieldT> *out_1_r_bits;
        digest_variable<FieldT> *out_1_cm_bits;

        // Packers
        multipacking_gadget<FieldT> *rt_packer;
        multipacking_gadget<FieldT> *in_0_sn_packer;
        multipacking_gadget<FieldT> *in_1_sn_packer;
        multipacking_gadget<FieldT> *out_0_cm_packer;
        multipacking_gadget<FieldT> *out_1_cm_packer;

        // Note gadgets
        input_note_gadget <FieldT, CommitmentHashT, MerkleTreeHashT> *in_0_note_gadget;
        input_note_gadget <FieldT, CommitmentHashT, MerkleTreeHashT> *in_1_note_gadget;
        cm_full_gadget<FieldT, CommitmentHashT> *out_0_cm_gadget;
        cm_full_gadget<FieldT, CommitmentHashT> *out_1_cm_gadget;
    };

    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    NewTransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
    make_new_transfer_circuit(size_t tree_height);
}

#include "NewTransferCircuit.tcc"

#endif //ZKTRADE_NEWTRANSFERCIRCUIT_HPP
