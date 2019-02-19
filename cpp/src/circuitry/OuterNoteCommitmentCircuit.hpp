#ifndef ZKTRADE_OUTERNOTECOMMITMENTCIRCUIT_H
#define ZKTRADE_OUTERNOTECOMMITMENTCIRCUIT_H

#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include "circuitry/gadgets/comms.hpp"

namespace zktrade {
    template<typename FieldT>
    struct OuterNoteCommitmentCircuit {
        protoboard<FieldT> *pb;
        pb_variable_array<FieldT> *k_packed;
        pb_variable<FieldT> *v_packed;
        pb_variable_array<FieldT> *cm_packed;
        pb_variable<FieldT> *ZERO;
        pb_variable_array<FieldT> *k_bits;
        pb_variable_array<FieldT> *v_bits;
        digest_variable<FieldT> *cm_bits;
        multipacking_gadget<FieldT>* k_packer;
        packing_gadget<FieldT>* v_packer;
        multipacking_gadget<FieldT>* cm_packer;
        outer_commitment_gadget<FieldT>* ocmg;
    };
    template<typename FieldT>
    OuterNoteCommitmentCircuit<FieldT> make_outer_note_commitment_circuit();
}

#include "OuterNoteCommitmentCircuit.tcc"

#endif //ZKTRADE_OUTERNOTECOMMITMENTCIRCUIT_H
