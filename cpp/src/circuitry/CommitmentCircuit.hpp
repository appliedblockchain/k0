#ifndef ZKTRADE_COMMITMENTCIRCUIT_HPP
#define ZKTRADE_COMMITMENTCIRCUIT_HPP

#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "circuitry/gadgets/comms.hpp"
#include "circuitry/gadgets/MTLeafAddition.hpp"

namespace zktrade {
    template<typename FieldT>
    struct CommitmentCircuit {
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

        outer_commitment_gadget<FieldT>* commitment_gadget;
    };
    template<typename FieldT>
    CommitmentCircuit<FieldT> make_commitment_circuit();
}

#include "CommitmentCircuit.tcc"

#endif //ZKTRADE_COMMITMENTCIRCUIT_HPP
