#ifndef K0_MTADDITIONCIRCUIT_HPP
#define K0_MTADDITIONCIRCUIT_HPP

#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>

using namespace libff;
using namespace libsnark;

namespace zktrade {
    template<typename FieldT, typename MerkleTreeHashT>
    struct MTAdditionCircuit {
        protoboard<FieldT> *pb;
        pb_variable_array<FieldT> *prev_root_packed;
        pb_variable<FieldT> *address_packed;
        pb_variable_array<FieldT> *next_leaf_packed;
        pb_variable_array<FieldT> *next_root_packed;

        pb_variable_array<FieldT> *address_bits;
        digest_variable<FieldT> *prev_leaf_bits;
        digest_variable<FieldT> *prev_root_bits;
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *prev_path_var;
        digest_variable<FieldT> *next_leaf_bits;
        digest_variable<FieldT> *next_root_bits;
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *next_path_var;

        multipacking_gadget<FieldT> *prev_root_packer;
        packing_gadget<FieldT> *address_packer;
        multipacking_gadget<FieldT> *next_leaf_packer;
        multipacking_gadget<FieldT> *next_root_packer;

        merkle_tree_check_update_gadget<FieldT, MerkleTreeHashT> *mt_update_gadget;
    };

    template<typename FieldT, typename MerkleTreeHashT>
    MTAdditionCircuit<FieldT, MerkleTreeHashT> make_mt_addition_circuit(size_t tree_height);
}


#include "MTAdditionCircuit.tcc"

#endif //K0_MTADDITIONCIRCUIT_HPP
