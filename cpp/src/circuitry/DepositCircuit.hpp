#ifndef ZKTRADE_DEPOSITCIRCUIT_HPP
#define ZKTRADE_DEPOSITCIRCUIT_HPP

#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include "circuitry/gadgets/comms.hpp"
#include "circuitry/gadgets/MTLeafAddition.hpp"

namespace zktrade {
    template<typename FieldT>
    struct DepositCircuit {

        typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;

        protoboard<FieldT> *pb;

        pb_variable_array<FieldT>* prev_root_packed;
        pb_variable<FieldT>* address_packed;
        pb_variable_array<FieldT> *k_packed;
        pb_variable<FieldT> *v_packed;
        pb_variable_array<FieldT> *cm_packed;
        pb_variable_array<FieldT>* next_root_packed;
        merkle_authentication_path_variable<FieldT, TwoToOneSHA256>* path_var;

        pb_variable<FieldT> *ZERO;
        digest_variable<FieldT>* prev_root_bits;
        pb_variable_array<FieldT> *address_bits;
        pb_variable_array<FieldT> *k_bits;
        pb_variable_array<FieldT> *v_bits;
        digest_variable<FieldT> *cm_bits;
        digest_variable<FieldT>* next_root_bits;

        multipacking_gadget<FieldT>* prev_root_packer;
        packing_gadget<FieldT>* address_packer;
        multipacking_gadget<FieldT>* k_packer;
        packing_gadget<FieldT>* v_packer;
        multipacking_gadget<FieldT>* cm_packer;
        multipacking_gadget<FieldT>* next_root_packer;

        outer_commitment_gadget<FieldT>* commitment_gadget;
        MTLeafAddition<FieldT, TwoToOneSHA256>* mt_addition_gadget;
    };
    template<typename FieldT>
    DepositCircuit<FieldT> make_deposit_circuit(size_t tree_height);
}

#include "DepositCircuit.tcc"

#endif //ZKTRADE_DEPOSITCIRCUIT_HPP
