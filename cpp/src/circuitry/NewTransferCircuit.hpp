#ifndef ZKTRADE_NEWTRANSFERCIRCUIT_HPP
#define ZKTRADE_NEWTRANSFERCIRCUIT_HPP


#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "circuitry/gadgets/input_note_gadget.hpp"
#include "circuitry/gadgets/cm.h"

namespace zktrade {
    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    struct NewTransferCircuit {
        protoboard<FieldT> *pb;

        // Primary inputs
        pb_variable_array<FieldT> *rt_packed;
        pb_variable_array<FieldT> *in_0_sn_packed;

        // Auxiliary inputs
        pb_variable<FieldT>* ZERO;
        digest_variable<FieldT> *rt_bits;
        pb_variable_array<FieldT> *in_0_v_bits;
        pb_variable_array<FieldT> *in_0_a_sk_bits;
        pb_variable_array<FieldT> *in_0_rho_bits;
        pb_variable_array<FieldT> *in_0_r_bits;
        pb_variable_array<FieldT> *in_0_address_bits;
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_0_path;
        shared_ptr<digest_variable<FieldT>> in_0_a_pk_bits;
        digest_variable<FieldT> *in_0_cm_bits;
        shared_ptr<digest_variable<FieldT>> in_0_sn_bits;

        // Unpackers
        multipacking_gadget<FieldT> *rt_packer;
        multipacking_gadget<FieldT> *in_0_sn_packer;

        input_note_gadget <FieldT, CommitmentHashT, MerkleTreeHashT> *note_gadget;
    };

    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    NewTransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
    make_new_transfer_circuit(size_t tree_height);
}

#include "NewTransferCircuit.tcc"

#endif //ZKTRADE_NEWTRANSFERCIRCUIT_HPP
