#ifndef ZKTRADE_TRANSFERCIRCUIT_HPP
#define ZKTRADE_TRANSFERCIRCUIT_HPP

#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "circuitry/gadgets/input_note_gadget.hpp"
#include "circuitry/gadgets/cm.h"

namespace zktrade {
    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    struct TransferCircuit {
        protoboard<FieldT> *pb;
        pb_variable_array<FieldT> *rt_packed;
        vector<pb_variable_array<FieldT> *> sn_in_packed_vec;
        vector<pb_variable_array<FieldT> *> cm_in_packed_vec;

        pb_variable<FieldT> *ZERO;
        digest_variable<FieldT> *rt_bits;

        // Input notes
        vector<pb_variable_array<FieldT> *> a_sk_in_bits_vec;
        vector<shared_ptr<digest_variable<FieldT>>> a_pk_in_bits_vec;
        vector<pb_variable_array<FieldT> *> rho_in_bits_vec;
        vector<pb_variable_array<FieldT> *> r_in_bits_vec;
        vector<pb_variable_array<FieldT> *> v_in_bits_vec;
        vector<digest_variable<FieldT> *> cm_in__bits_vec;
        vector<shared_ptr<digest_variable<FieldT>>> sn_in_bits_vec;
        vector<pb_variable_array<FieldT> *> address_in_bits_vec;
        vector<merkle_authentication_path_variable <
               FieldT, MerkleTreeHashT>*> path_in_vec;
        vector<multipacking_gadget<FieldT> *> sn_in_packer_vec;
        vector<input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT> *> input_note_vec;

        // Output notes
        vector<pb_variable_array<FieldT> *> a_pk_out_bits_vec;
        vector<pb_variable_array<FieldT> *> rho_out_bits_vec;
        vector<pb_variable_array<FieldT> *> r_out_bits_vec;
        vector<pb_variable_array<FieldT>*> v_out_bits_vec;
        vector<digest_variable<FieldT> *> cm_out_bits_vec;
        vector<cm_gadget<FieldT, CommitmentHashT> *> cm_out_gadget_vec;

        multipacking_gadget<FieldT> *rt_packer;
    };

    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
    make_transfer_circuit(size_t tree_height);
}

#include "TransferCircuit.tcc"

#endif //ZKTRADE_TRANSFERCIRCUIT_HPP
