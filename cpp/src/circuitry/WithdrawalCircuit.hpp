#ifndef ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_HPP
#define ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_HPP

#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include "gadgets/cm.h"
#include "gadgets/prfs.hpp"
#include "util.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

template<typename FieldT>
struct WithdrawalCircuit
{
    protoboard<FieldT>* pb;

    // Primary inputs
    pb_variable_array<FieldT>* root_packed;
    pb_variable<FieldT>* v_packed;
    pb_variable_array<FieldT>* sn_packed;

    // Auxiliary inputs
    digest_variable<FieldT>* root_bits;
    pb_variable_array<FieldT>* address_bits;
    pb_variable_array<FieldT>* a_sk_bits;
    shared_ptr<digest_variable<FieldT>> a_pk_bits;
    pb_variable_array<FieldT>* v_bits;
    pb_variable_array<FieldT>* rho_bits;
    pb_variable_array<FieldT>* r_bits;
    digest_variable<FieldT>* commitment_bits;
    shared_ptr<digest_variable<FieldT>> sn_bits;
    merkle_authentication_path_variable<FieldT, sha256_two_to_one_hash_gadget<FieldT>>* path;

    // Unpackers
    multipacking_gadget<FieldT>* root_packer;
    packing_gadget<FieldT>* v_packer;
    multipacking_gadget<FieldT>* sn_packer;

    // Workhorse gadgets
    prf_addr_gadget<FieldT>* addr_gadget;
    cm_gadget<FieldT>* commitment_gadget;
    prf_sn_gadget<FieldT>* sn_gadget;
    merkle_tree_check_read_gadget<FieldT, sha256_two_to_one_hash_gadget<FieldT>>* mt_path_gadget;
};

template<typename FieldT>
WithdrawalCircuit<FieldT> make_withdrawal_circuit(size_t tree_height);

#include "WithdrawalCircuit.tcc"

#endif //ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_HPP
