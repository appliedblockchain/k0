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

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
struct WithdrawalCircuit {
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

    // Workhorse gadgets
    prf_addr_gadget <FieldT, CommitmentHashT> *addr_gadget;
    cm_gadget <FieldT, CommitmentHashT> *commitment_gadget;
    prf_sn_gadget <FieldT, CommitmentHashT> *sn_gadget;
    merkle_tree_check_read_gadget<FieldT, MerkleTreeHashT> *mt_path_gadget;
};

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
WithdrawalCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> make_withdrawal_circuit(size_t tree_height);

#include "WithdrawalCircuit.tcc"

#endif //ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_HPP
