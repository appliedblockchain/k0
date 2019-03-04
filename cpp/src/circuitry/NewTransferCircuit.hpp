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
        pb_variable<FieldT> *v_packed;
        pb_variable_array<FieldT> *sn_packed;
        pb_variable<FieldT>* recipient_public;

        // Auxiliary inputs
        pb_variable<FieldT>* ZERO;
        digest_variable<FieldT> *rt_bits;
        pb_variable_array<FieldT> *v_bits;
        pb_variable_array<FieldT> *a_sk_bits;
        pb_variable_array<FieldT> *rho_bits;
        pb_variable_array<FieldT> *r_bits;
        pb_variable_array<FieldT> *address_bits;
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *path;
        pb_variable<FieldT>* recipient_private;
        shared_ptr<digest_variable<FieldT>> a_pk_bits;
        digest_variable<FieldT> *commitment_bits;
        shared_ptr<digest_variable<FieldT>> sn_bits;

        // Unpackers
        multipacking_gadget<FieldT> *rt_packer;
        packing_gadget<FieldT> *v_packer;
        multipacking_gadget<FieldT> *sn_packer;

        input_note_gadget <FieldT, CommitmentHashT, MerkleTreeHashT> *note_gadget;
    };

    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    NewTransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
    make_new_transfer_circuit(size_t tree_height);
}

#include "NewTransferCircuit.tcc"

#endif //ZKTRADE_NEWTRANSFERCIRCUIT_HPP
