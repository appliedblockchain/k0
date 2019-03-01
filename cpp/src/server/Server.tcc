#include <stdlib.h>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>
#include "Server.hpp"
#include "../pkutil.cpp"
#include "../serialization.hpp"
#include "../printbits.hpp"
#include "../json_conversion.hpp"
#include "../packing.hpp"
#include "../util.h"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::Server(size_t height,
                                                                  AbstractServerConnector &connector,
                                                                  serverVersion_t type)
        : ZKTradeStubServer(connector, type),
          tree_height{height},
          mt{height} {}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::reset() {
    mt = MerkleTree<MerkleTreeHashT>{tree_height};
    return bits2hex(mt.root());
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::root() {
    return bits2hex(mt.root());
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setCommitmentPk(
        string pk_path) {
    commitment_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
            pk_path);
    commitment_pk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setCommitmentVk(
        string vk_path) {
    commitment_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
            vk_path);
    commitment_vk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setAdditionPk(
        string pk_path) {
    addition_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
            pk_path);
    addition_pk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setAdditionVk(
        string vk_path) {
    addition_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
            vk_path);
    addition_vk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setTransferPk(
        string pk_path) {
    transfer_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
            pk_path);
    transfer_pk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setTransferVk(
        string vk_path) {
    transfer_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
            vk_path);
    transfer_vk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setWithdrawalPk(
        string pk_path) {
    withdrawal_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
            pk_path);
    withdrawal_pk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setWithdrawalVk(
        string vk_path) {
    withdrawal_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
            vk_path);
    withdrawal_vk_loaded = true;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::add(
        const string &leaf_hex) {
    bit_vector leaf_bv = hex2bits(leaf_hex);
    uint address = mt.add(leaf_bv);
    mt.print();
    Json::Value result;
    result["address"] = address;
    result["newRoot"] = bits2hex(mt.root());
    return result;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::element(
        int address) {
    if (mt.num_elements() == 0 || address > (mt.num_elements() - 1)) {
        throw JsonRpcException(-32602, "Address too big");
    }
    return bits2hex(mt[address]);
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
std::string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::hash(
        const std::string &left_hex,
        const std::string &right_hex) {
    libff::bit_vector block = hex2bits(left_hex);
    libff::bit_vector right = hex2bits(right_hex);
    block.insert(block.end(), right.begin(), right.end());
    return bits2hex(MerkleTreeHashT::get_hash(block));
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prepare_deposit(
        const std::string &a_pk_str,
        const std::string &rho_str,
        const std::string &r_str,
        const std::string &v_str) {
    bit_vector a_pk_bits = hex2bits(a_pk_str);
    bit_vector rho_bits = hex2bits(rho_str);
    bit_vector r_bits = hex2bits(r_str);
    FieldT v = FieldT(v_str.c_str());
    bit_vector v_bits = field_element_to_64_bits(v);

    auto k_bits = comm_r<CommitmentHashT>(a_pk_bits, rho_bits, r_bits);
    auto cm_bits = comm_s<CommitmentHashT>(k_bits, v_bits);


    auto sim_result = mt.simulate_add(cm_bits);

    uint address = mt.num_elements();

    auto comm_circuit = make_commitment_circuit<FieldT, CommitmentHashT>();
    // Generate deposit proof
    comm_circuit.k_bits->fill_with_bits(*comm_circuit.pb, k_bits);
    comm_circuit.k_packer->generate_r1cs_witness_from_bits();
    comm_circuit.pb->val(*comm_circuit.v_packed) = v;
    comm_circuit.v_packer->generate_r1cs_witness_from_packed();
    assert(!comm_circuit.pb->is_satisfied());

    comm_circuit.commitment_gadget->generate_r1cs_witness();
    comm_circuit.cm_packer->generate_r1cs_witness_from_bits();
    assert(comm_circuit.pb->is_satisfied());
    assert(comm_circuit.cm_bits->get_digest() == cm_bits);

    if (!comm_circuit.pb->is_satisfied()) {
        throw JsonRpcException(-32000, "Commitment circuit not satisfied");
    }

    auto add_circuit = make_mt_addition_circuit<FieldT, MerkleTreeHashT>(
            tree_height);

    add_circuit.prev_root_bits->generate_r1cs_witness(mt.root());
    add_circuit.prev_root_packer->generate_r1cs_witness_from_bits();

    add_circuit.pb->val(*add_circuit.address_packed) = address;
    add_circuit.address_packer->generate_r1cs_witness_from_packed();

    add_circuit.prev_path_var->generate_r1cs_witness(address, mt.path(address));

    add_circuit.next_leaf_bits->generate_r1cs_witness(cm_bits);
    add_circuit.next_leaf_packer->generate_r1cs_witness_from_bits();

    add_circuit.next_root_bits->generate_r1cs_witness(get<1>(sim_result));
    add_circuit.next_root_packer->generate_r1cs_witness_from_bits();

    assert(!add_circuit.pb->is_satisfied());
    add_circuit.mt_update_gadget->generate_r1cs_witness();

    assert(add_circuit.pb->is_satisfied());
    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> comm_proof =
            r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
                    commitment_pk, comm_circuit.pb->primary_input(),
                    comm_circuit.pb->auxiliary_input());

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> add_proof =
            r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
                    addition_pk, add_circuit.pb->primary_input(),
                    add_circuit.pb->auxiliary_input());

    bool comm_verified =
            r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
                    commitment_vk, comm_circuit.pb->primary_input(),
                    comm_proof);
    if (comm_verified) {
        cout << "Commitment proof successfully verified." << endl;
    } else {
        cerr << "Commitment proof verification failed." << endl;
    }

    cout << "ADDITION params" << endl << hex << add_circuit.pb->primary_input()
         << endl;
    bool add_verified =
            r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
                    addition_vk, add_circuit.pb->primary_input(), add_proof);
    if (add_verified) {
        cout << "Addition proof successfully verified." << endl;
    } else {
        cerr << "Addition proof verification failed." << endl;
    }


    Json::Value result;
    result["k"] = bits_to_hex(k_bits);
    result["cm"] = bits_to_hex(cm_bits);
    result["nextRoot"] = bits_to_hex(get<1>(sim_result));

    result["address"] = (uint) address;
    result["commitmentProof"] = json_conversion::to_json(comm_proof);
    result["additionProof"] = json_conversion::to_json(add_proof);

    return result;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prepare_transfer(
        const std::string &input_0_address_str,
        const std::string &input_0_a_sk_str, const std::string &input_0_rho_str,
        const std::string &input_0_r_str, const std::string &input_0_v_str,
        const std::string &input_1_address_str,
        const std::string &input_1_a_sk_str, const std::string &input_1_rho_str,
        const std::string &input_1_r_str, const std::string &input_1_v_str,
        const std::string &output_0_a_pk_str,
        const std::string &output_0_rho_str, const std::string &output_0_r_str,
        const std::string &output_0_v_str, const std::string &output_1_a_pk_str,
        const std::string &output_1_rho_str, const std::string &output_1_r_str,
        const std::string &output_1_v_str) {
    Json::Value result;
    result["foo"] = "bar";
    return result;
}


template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prepare_withdrawal(
        const std::string &address_dec_str,
        const std::string &a_sk_hex_str,
        const std::string &rho_hex_str,
        const std::string &r_hex_str,
        const std::string &v_hex_str,
        const std::string &recipient_hex_str) {
    unsigned long address = strtoul(address_dec_str.c_str(), NULL, 10);
    bit_vector a_sk_bits = hex2bits(a_sk_hex_str);
    bit_vector rho_bits = hex2bits(rho_hex_str);
    bit_vector r_bits = hex2bits(r_hex_str);
    FieldT v = FieldT(v_hex_str.c_str());
    bit_vector v_bits = field_element_to_64_bits(v);
    auto recipient_dec_str = hex_to_dec_string(recipient_hex_str);
    auto recipient = FieldT(recipient_dec_str.c_str());

    auto wd_circuit =
            make_withdrawal_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(
                    tree_height);
    wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
    wd_circuit.pb->val(*wd_circuit.v_packed) = v;
    wd_circuit.pb->val(*wd_circuit.recipient_public) = recipient;
    wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
    wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, a_sk_bits);
    wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, rho_bits);
    wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, r_bits);
    wd_circuit.address_bits->fill_with_bits_of_ulong(*wd_circuit.pb, address);
    wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
    wd_circuit.pb->val(*wd_circuit.recipient_private) = recipient;

    wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
    wd_circuit.v_packer->generate_r1cs_witness_from_packed();
    wd_circuit.addr_gadget->generate_r1cs_witness();
    wd_circuit.commitment_gadget->generate_r1cs_witness();

    assert(!wd_circuit.pb->is_satisfied());
    wd_circuit.mt_path_gadget->generate_r1cs_witness();
    wd_circuit.sn_gadget->generate_r1cs_witness();
    wd_circuit.sn_packer->generate_r1cs_witness_from_bits();

    assert(wd_circuit.pb->is_satisfied());

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
            r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
                    withdrawal_pk, wd_circuit.pb->primary_input(),
                    wd_circuit.pb->auxiliary_input());

    bool verified =
            r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
                    withdrawal_vk, wd_circuit.pb->primary_input(), proof);
    if (verified) {
        cout << "Withdrawal proof successfully verified." << endl;
    } else {
        cerr << "Withdrawal proof verification failed." << endl;
    }

    cout << "WITHDRAWAL PUBLIC INPUT" << endl << hex
         << wd_circuit.pb->primary_input() << endl;


    Json::Value result;
    result["sn"] = bits_to_hex(wd_circuit.sn_bits->get_digest());
    result["proof"] = json_conversion::to_json(proof);
    return result;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prf_addr(
        const string &a_sk_hex) {
    bit_vector a_sk = hex2bits(a_sk_hex);
    bit_vector a_pk = zktrade::prf_addr<CommitmentHashT>(a_sk);
    string a_pk_hex = bits2hex(a_pk);
    return a_pk_hex;
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::status() {
    const bool ready = commitment_pk_loaded & commitment_vk_loaded &
                       addition_pk_loaded & addition_vk_loaded &
                       transfer_pk_loaded & transfer_vk_loaded &
                       withdrawal_pk_loaded & withdrawal_vk_loaded;
    Json::Value result;
    result["ready"] = ready;
    return result;
}