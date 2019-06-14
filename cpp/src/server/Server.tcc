#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <stdlib.h>
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/ExampleCircuit.hpp"
#include "circuitry/MTAdditionCircuit.hpp"
#include "circuitry/TransferCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "json_conversion.hpp"
#include "packing.hpp"
#include "pkutil.cpp"
#include "printbits.hpp"
#include "scheme/comms.hpp"
#include "scheme/note_encryption.hpp"
#include "scheme/ka.hpp"
#include "scheme/prfs.h"
#include "serialization.hpp"
#include "Server.hpp"
#include "util.h"
#include "proof_serialization.hpp"

#include <iostream>
#include <chrono>
#include <ctime>


template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::Server(
    size_t height,
    AbstractServerConnector &connector,
    serverVersion_t type)
    : ZKTradeStubServer(connector, type),
      tree_height{height},
      mt{height}
{
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::reset()
{
    mt = MerkleTree<MerkleTreeHashT>{tree_height};
    return bits2hex(mt.root());
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::root()
{
    return bits2hex(mt.root());
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setCommitmentPk(
    string pk_path)
{
    commitment_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
        pk_path);
    commitment_pk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setCommitmentVk(
    string vk_path)
{
    commitment_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
        vk_path);
    commitment_vk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setAdditionPk(
    string pk_path)
{
    addition_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
        pk_path);
    addition_pk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setAdditionVk(
    string vk_path)
{
    addition_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
        vk_path);
    addition_vk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setTransferPk(
    string pk_path)
{
    transfer_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
        pk_path);
    transfer_pk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setTransferVk(
    string vk_path)
{
    transfer_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
        vk_path);
    transfer_vk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setWithdrawalPk(
    string pk_path)
{
    withdrawal_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
        pk_path);
    withdrawal_pk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setWithdrawalVk(
    string vk_path)
{
    withdrawal_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
        vk_path);
    withdrawal_vk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setExamplePk(
    string pk_path)
{
    example_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
        pk_path);
    example_pk_loaded = true;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::setExampleVk(
    string vk_path)
{
    example_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
        vk_path);
    example_vk_loaded = true;
}


template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::add(
    const string &leaf_hex)
{
    cout << "leaf " << leaf_hex << endl;
    bit_vector leaf_bv = hex2bits(leaf_hex);
    cout << "mt root before" << bits2hex(mt.root()) << endl;
    cout << "mt adding " << bits2hex(leaf_bv) << endl;
    uint address = mt.add(leaf_bv);
    cout << "mt root after" << bits2hex(mt.root()) << endl;
    Json::Value result;
    result["address"] = address;
    result["nextRoot"] = bits2hex(mt.root());
    return result;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
std::string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::cm(
    const std::string &a_pk_hex_str, const std::string &rho_hex_str,
    const std::string &r_hex_str, const std::string &v_dec_str)
{
    bit_vector a_pk = hex2bits(a_pk_hex_str);
    bit_vector rho = hex2bits(rho_hex_str);
    bit_vector r = hex2bits(r_hex_str);
    FieldT v = FieldT(v_dec_str.c_str());
    bit_vector v_bits = field_element_to_64_bits(v);
    bit_vector k = comm_r<CommitmentHashT>(a_pk, rho, r);
    bit_vector commitment = comm_s<CommitmentHashT>(k, v_bits);
    return bits2hex(commitment);
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::decrypt_note(
    const std::string& combined_ciphertext_hex_str,
    const std::string& sk_enc_hex_str,
    const std::string& pk_enc_hex_str)
{
    if (combined_ciphertext_hex_str.length() != 2 + (32 + 104) * 2) {
        throw JsonRpcException(-32602, "Invalid ciphertext length");
    }
    if (sk_enc_hex_str.length() != 2 + 32 * 2) {
        throw JsonRpcException(-32602, "Invalid sk_enc length");
    }
    if (pk_enc_hex_str.length() != 2 + 32 * 2) {
        throw JsonRpcException(-32602, "Invalid pk_enc length");
    }

    unsigned char combined_ciphertext[136];
    fill_with_bytes_of_hex_string(combined_ciphertext, combined_ciphertext_hex_str);
    unsigned char epk[32];
    memcpy(epk, combined_ciphertext, 32);
    unsigned char ciphertext[104];
    memcpy(ciphertext, combined_ciphertext + 32, 104);
    unsigned char sk_enc[32];
    fill_with_bytes_of_hex_string(sk_enc, sk_enc_hex_str);
    unsigned char pk_enc[32];
    fill_with_bytes_of_hex_string(pk_enc, pk_enc_hex_str);

    unsigned char decrypted_text[88];
    Json::Value res;
    if (zktrade::decrypt_note(decrypted_text, epk, ciphertext, sk_enc, pk_enc) == 0) {
        res["success"] = true;
        res["value"] = bytes_to_hex(decrypted_text, 88);
    } else {
        res["success"] = false;
    }
    return res;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::deriveKeys(
    const std::string& a_sk_hex)
{
    bit_vector a_sk = hex2bits(a_sk_hex);
    bit_vector a_pk = zktrade::prf_addr_a_pk<CommitmentHashT>(a_sk);

    unsigned char sk_enc[32];
    auto prfed = prf_addr_sk_enc<CommitmentHashT>(a_sk);
    fill_with_bits(sk_enc, prfed);
    ka_format_private(sk_enc);
    unsigned char pk_enc[32];
    ka_derive_public(pk_enc, sk_enc);

    Json::Value res;
    res["a_pk"] = bits2hex(a_pk);
    res["sk_enc"] = bytes_to_hex(sk_enc, 32);
    res["pk_enc"] = bytes_to_hex(pk_enc, 32);
    return res;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::element(
    int address)
{
    if (mt.num_elements() == 0 || address > (mt.num_elements() - 1))
    {
        throw JsonRpcException(-32602, "Address too big");
    }
    return bits2hex(mt[address]);
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::encrypt_note(
    const string& plaintext_hex_str, const string& pk_enc_hex_str)
{
    if (plaintext_hex_str.length() != 2 + 88 * 2) {
        throw JsonRpcException(-32602, "Invalid plaintext length");
    }
    if (pk_enc_hex_str.length() != 2 + 32 * 2) {
        throw JsonRpcException(-32602, "Invalid pk_enc length");
    }

    unsigned char plaintext[88];
    fill_with_bytes_of_hex_string(plaintext, plaintext_hex_str);
    unsigned char pk_enc[32];
    fill_with_bytes_of_hex_string(pk_enc, pk_enc_hex_str);

    unsigned char epk[32];
    unsigned char ciphertext[104];

    if (zktrade::encrypt_note(epk, ciphertext, plaintext, pk_enc) == 0) {
        unsigned char combined_ciphertext[136];
        memcpy(combined_ciphertext, epk, 32);
        memcpy(combined_ciphertext+32, ciphertext, 104);
        return bytes_to_hex(combined_ciphertext, 136);
    } else {
        throw JsonRpcException(-32010, "Encryption failed.");
    }
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::exampleWitnessAndProof(
    const std::string &x)
{
    ExampleCircuit<FieldT> circuit;
    circuit.pb.val(circuit.x) = FieldT(x.c_str());
    circuit.example_gadget->generate_r1cs_witness();
    if (circuit.pb.is_satisfied()) {
        cout << "SATISFIED" << endl;
    } else {
        cout << "NOT SATISFIED" << endl;
    }

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
        r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
            example_pk, circuit.pb.primary_input(),
            circuit.pb.auxiliary_input());

    bool verified =
        r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
            example_vk, circuit.pb.primary_input(),
            proof);
    if (verified)
    {
        cout << "Example proof successfully verified." << endl;
    }
    else
    {
        cerr << "Example proof verification failed." << endl;
        throw JsonRpcException(-32010, "Example proof verification failed.");
    }

    cout << "PROOF RAW" << endl;
    cout << proof << endl;

    Json::Value res;
    res["out"] = field_element_to_string(circuit.pb.val(circuit.out));
    res["proofAffine"] = json_conversion::proof_to_json_affine(proof);
    res["proofJacobian"] = json_conversion::proof_to_json_jacobian(proof);

    cout << "PROOF JSON" << endl;
    cout << res["proofJacobian"] << endl;

    return res;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
std::string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::hash(
    const std::string &left_hex,
    const std::string &right_hex)
{
    libff::bit_vector block = hex2bits(left_hex);
    libff::bit_vector right = hex2bits(right_hex);
    block.insert(block.end(), right.begin(), right.end());
    return bits2hex(MerkleTreeHashT::get_hash(block));
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::depositCommitmentProof(
    const std::string &a_pk_str,
    const std::string &rho_str,
    const std::string &r_str,
    const std::string &v_str)
{
    bit_vector a_pk_bits = hex2bits(a_pk_str);
    bit_vector rho_bits = hex2bits(rho_str);
    bit_vector r_bits = hex2bits(r_str);
    FieldT v = FieldT(v_str.c_str());
    bit_vector v_bits = field_element_to_64_bits(v);

    auto k_bits = comm_r<CommitmentHashT>(a_pk_bits, rho_bits, r_bits);
    auto cm_bits = comm_s<CommitmentHashT>(k_bits, v_bits);
    auto circuit = make_commitment_circuit<FieldT, CommitmentHashT>();

    // Generate deposit proof
    circuit.k_bits->fill_with_bits(*circuit.pb, k_bits);
    circuit.k_packer->generate_r1cs_witness_from_bits();
    circuit.pb->val(*circuit.v_packed) = v;
    circuit.v_packer->generate_r1cs_witness_from_packed();
    assert(!circuit.pb->is_satisfied());

    circuit.commitment_gadget->generate_r1cs_witness();
    circuit.cm_packer->generate_r1cs_witness_from_bits();
    assert(circuit.pb->is_satisfied());
    assert(circuit.cm_bits->get_digest() == cm_bits);

    if (!circuit.pb->is_satisfied())
    {
        throw JsonRpcException(-32010, "Commitment circuit not satisfied");
    }

    cout << "CIRCUIT" << endl;
    cout << "k  " << bits2hex(circuit.k_bits->get_bits(*circuit.pb))
         << endl;
    cout << "cm " << bits2hex(circuit.cm_bits->get_digest()) << endl;
    cout << "CHECKING COMMITMENT" << endl;
    cout << circuit.pb->primary_input().size() << " "
         << commitment_pk.constraint_system.num_inputs() << endl;

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
        r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
            commitment_pk, circuit.pb->primary_input(),
            circuit.pb->auxiliary_input());

    bool verified =
        r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
            commitment_vk, circuit.pb->primary_input(),
            proof);

    if (verified)
    {
        cout << "Commitment proof successfully verified." << endl;
    }
    else
    {
        cerr << "Commitment proof verification failed." << endl;
        throw JsonRpcException(-32010, "Commitment proof verification failed.");
    }

    Json::Value result;
    result["k"] = bits_to_hex(k_bits);
    result["cm"] = bits_to_hex(cm_bits);
    result["proof_affine"] = json_conversion::proof_to_json_affine(proof);
    result["proof_jacobian"] = json_conversion::proof_to_json_jacobian(proof);

    return result;
}
template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::merkleTreeAdditionProof(
    const std::string &prev_root_hex,
    const std::string &address_dec,
    const std::string &leaf_hex,
    const Json::Value &path,
    const std::string &next_root_hex)
{

    vector<bit_vector> path_vec(path.size());
    for (uint i = 0; i < path.size(); i++)
    {
        path_vec[i] = hex2bits(path[i].asString());
    }

    auto circuit = make_mt_addition_circuit<FieldT, MerkleTreeHashT>(
        tree_height);

    circuit.prev_root_bits->generate_r1cs_witness(hex2bits(prev_root_hex));
    circuit.prev_root_packer->generate_r1cs_witness_from_bits();

    circuit.pb->val(*circuit.address_packed) = FieldT(address_dec.c_str());
    circuit.address_packer->generate_r1cs_witness_from_packed();

    circuit.prev_path_var->generate_r1cs_witness(stoi(address_dec), path_vec);

    circuit.next_leaf_bits->generate_r1cs_witness(hex2bits(leaf_hex));
    circuit.next_leaf_packer->generate_r1cs_witness_from_bits();

    circuit.next_root_bits->generate_r1cs_witness(hex2bits(next_root_hex));
    circuit.next_root_packer->generate_r1cs_witness_from_bits();

    assert(!circuit.pb->is_satisfied());
    circuit.mt_update_gadget->generate_r1cs_witness();
    assert(circuit.pb->is_satisfied());

    if (!circuit.pb->is_satisfied())
    {
        throw JsonRpcException(-32010, "Addition circuit not satisfied");
    }


    //TODO rename vars
    auto start = std::chrono::system_clock::now();
    std::time_t readableStart = std::chrono::system_clock::to_time_t(start);
    cout << "Called at: " << std::ctime(&readableStart) << endl;

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
        r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
            addition_pk, circuit.pb->primary_input(),
            circuit.pb->auxiliary_input());
    cout << "ADDITION params" << endl
         << dec << circuit.pb->primary_input()
         << endl;
    bool verified =
        r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
            addition_vk, circuit.pb->primary_input(), proof);
    if (verified)
    {
        cout << "Addition proof successfully verified." << endl;
    }
    else
    {
        cerr << "Addition proof verification failed." << endl;
        throw JsonRpcException(-32010, "Addition proof verification failed.");
    }

    Json::Value result;
    result["proof_affine"] = json_conversion::proof_to_json_affine(proof);
    result["proof_jacobian"] = json_conversion::proof_to_json_jacobian(proof);
    return result;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::pack256Bits(
    const std::string& input
    )
{
    auto packed = pack<FieldT>(hex2bits(input));
    Json::Value result;
    result[0] = field_element_to_string(packed[0]);
    result[1] = field_element_to_string(packed[1]);
    return result;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prepareTransfer(
    const std::string &prev_root_hex,
    const std::string &input_0_address_str,
    const std::string &input_0_a_sk_str,
    const std::string &input_0_rho_str,
    const std::string &input_0_r_str,
    const std::string &input_0_v_str,
    const Json::Value &input_0_path,
    const std::string &input_1_address_str,
    const std::string &input_1_a_sk_str,
    const std::string &input_1_rho_str,
    const std::string &input_1_r_str,
    const std::string &input_1_v_str,
    const Json::Value &input_1_path,
    const std::string &output_0_a_pk_str,
    const std::string &output_0_rho_str,
    const std::string &output_0_r_str,
    const std::string &output_0_v_str,
    const std::string &output_1_a_pk_str,
    const std::string &output_1_rho_str,
    const std::string &output_1_r_str,
    const std::string &output_1_v_str,
    const std::string &callee_hex_str)
{

    cout << "MAX " << dec << UINT64_MAX << endl;

    // TODO validate inputs (max values for bit lengths)

    auto in_0_address = strtoul(input_0_address_str.c_str(), NULL, 10);
    auto in_1_address = strtoul(input_1_address_str.c_str(), NULL, 10);

    vector<bit_vector> in_0_path_vec(input_0_path.size());
    for (uint i = 0; i < input_0_path.size(); i++)
    {
        in_0_path_vec[i] = hex2bits(input_0_path[i].asString());
    }
    vector<bit_vector> in_1_path_vec(input_1_path.size());
    for (uint i = 0; i < input_1_path.size(); i++)
    {
        in_1_path_vec[i] = hex2bits(input_1_path[i].asString());
    }

    input_note in[]{
        {in_0_address,
                hex2bits(input_0_a_sk_str),
                hex2bits(input_0_rho_str),
                hex2bits(input_0_r_str),
                strtoul(input_0_v_str.c_str(), NULL, 10),
                in_0_path_vec},
        {in_1_address,
                hex2bits(input_1_a_sk_str),
                hex2bits(input_1_rho_str),
                hex2bits(input_1_r_str),
                strtoul(input_1_v_str.c_str(), NULL, 10),
                in_1_path_vec}};

    output_note out[]{
        {
            hex2bits(output_0_a_pk_str),
                hex2bits(output_0_rho_str),
                hex2bits(output_0_r_str),
                strtoul(output_0_v_str.c_str(), NULL, 10),
                },
        {
            hex2bits(output_1_a_pk_str),
                hex2bits(output_1_rho_str),
                hex2bits(output_1_r_str),
                strtoul(output_1_v_str.c_str(), NULL, 10),
                }};

    auto callee_dec_str = hex_to_dec_string(callee_hex_str);
    auto callee = FieldT(callee_dec_str.c_str());

    auto xfer_circuit = make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(
        tree_height);
    bit_vector mt_root = hex2bits(prev_root_hex);
    cout << "got root" << bits2hex(mt_root) << endl;
    populate(xfer_circuit, tree_height, mt_root, in[0], in[1], out[0],
             out[1], callee);

    print(xfer_circuit);
    for (size_t i = 0; i < 2; i++)
    {
        input_note c = in[i];
        cout << "Input note " << i << endl;
        cout << "address: " << c.address << endl;
        cout << "a_sk: " << bits2hex(c.a_sk) << endl;
        cout << "rho: " << bits2hex(c.rho) << endl;
        cout << "r: " << bits2hex(c.r) << endl;
        cout << "v: " << c.v << endl;
        cout << "MT path" << endl;
        for (auto x : c.path)
        {
            cout << bits2hex(x) << endl;
        }
        cout << endl;
    }

    for (size_t i = 0; i < 2; i++)
    {
        output_note c = out[i];
        cout << "Output note " << i << endl;
        cout << "p_sk: " << bits2hex(c.a_pk) << endl;
        cout << "rho: " << bits2hex(c.rho) << endl;
        cout << "r: " << bits2hex(c.r) << endl;
        cout << "v: " << c.v << endl;
        cout << endl;
    }

    cout << "Root after 1 " << bits2hex(xfer_circuit.rt_bits->get_digest())
         << endl;

    generate_witness(xfer_circuit);

    cout << "Root after 2 " << bits2hex(xfer_circuit.rt_bits->get_digest())
         << endl;

    print(xfer_circuit);
    if (!xfer_circuit.pb->is_satisfied())
    {
        throw JsonRpcException(-32010, "Transfer circuit not satisfied");
    }

    cout << "TRANSFER PUBLIC INPUT" << endl
         << hex
         << xfer_circuit.pb->primary_input() << endl;

    auto xfer_proof =
        r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
            transfer_pk, xfer_circuit.pb->primary_input(),
            xfer_circuit.pb->auxiliary_input());

    bool xfer_verified =
        r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
            transfer_vk, xfer_circuit.pb->primary_input(),
            xfer_proof);
    if (xfer_verified)
    {
        cout << "Transfer proof successfully verified." << endl;
    }
    else
    {
        cerr << "Transfer proof verification failed." << endl;
        throw JsonRpcException(-32010, "Transfer proof verification failed.");
    }

    cout << "TRANSFER PUBLIC INPUTS" << endl;
    cout << xfer_circuit.pb->primary_input() << endl;

    Json::Value result;
    result["input_0_sn"] = bits2hex(
        xfer_circuit.in_0_sn_bits->get_digest());
    result["input_1_sn"] = bits2hex(
        xfer_circuit.in_1_sn_bits->get_digest());
    result["output_0_cm"] = bits2hex(
        xfer_circuit.out_0_cm_bits->get_digest());
    result["output_1_cm"] = bits2hex(
        xfer_circuit.out_1_cm_bits->get_digest());
    result["proof_affine"] = json_conversion::proof_to_json_affine(xfer_proof);
    result["proof_jacobian"] = json_conversion::proof_to_json_jacobian(xfer_proof);
    return result;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prepare_withdrawal(
    const std::string &address_dec_str,
    const std::string &a_sk_hex_str,
    const std::string &rho_hex_str,
    const std::string &r_hex_str,
    const std::string &v_dec_str,
    const std::string &recipient_hex_str)
{
    unsigned long address = strtoul(address_dec_str.c_str(), NULL, 10);
    bit_vector address_bits = int_to_bits<FieldT>(address, tree_height);
    bit_vector a_sk_bits = hex2bits(a_sk_hex_str);
    bit_vector rho_bits = hex2bits(rho_hex_str);
    bit_vector r_bits = hex2bits(r_hex_str);
    FieldT v = FieldT(v_dec_str.c_str());
    bit_vector v_bits = field_element_to_64_bits(v);
    auto recipient_dec_str = hex_to_dec_string(recipient_hex_str);
    auto recipient = FieldT(recipient_dec_str.c_str());

    cout << "Coin" << endl;
    cout << "address " << dec << address << endl;
    cout << "a_sk " << bits2hex(a_sk_bits) << endl;
    cout << "rho " << bits2hex(rho_bits) << endl;
    cout << "r " << bits2hex(r_bits) << endl;
    cout << "v " << v << endl;

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
    wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
    wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
    wd_circuit.pb->val(*wd_circuit.recipient_private) = recipient;
    wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
    wd_circuit.v_packer->generate_r1cs_witness_from_packed();

    wd_circuit.note_gadget->generate_r1cs_witness();
    wd_circuit.sn_packer->generate_r1cs_witness_from_bits();

    if (!wd_circuit.pb->is_satisfied())
    {
        throw JsonRpcException(-32010, "Withdrawal circuit not satisfied");
    }

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
        r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
            withdrawal_pk, wd_circuit.pb->primary_input(),
            wd_circuit.pb->auxiliary_input());

    bool verified =
        r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
            withdrawal_vk, wd_circuit.pb->primary_input(), proof);
    if (verified)
    {
        cout << "Withdrawal proof successfully verified." << endl;
    }
    else
    {
        cerr << "Withdrawal proof verification failed." << endl;
        throw JsonRpcException(-32010, "Withdrawal proof verification failed.");
    }

    cout << "WITHDRAWAL PUBLIC INPUT" << endl
         << hex
         << wd_circuit.pb->primary_input() << endl;

    Json::Value result;
    result["sn"] = bits_to_hex(wd_circuit.sn_bits->get_digest());
    result["proof"] = json_conversion::proof_to_json_affine(proof);
    return result;
}

// TODO remove (a_pk is also generated in deriveKeys)
template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::prf_addr(
    const string &a_sk_hex)
{
    bit_vector a_sk = hex2bits(a_sk_hex);
    bit_vector a_pk = zktrade::prf_addr_a_pk<CommitmentHashT>(a_sk);
    string a_pk_hex = bits2hex(a_pk);
    return a_pk_hex;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
Json::Value
zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::status()
{
    const bool ready = commitment_pk_loaded & commitment_vk_loaded &
        addition_pk_loaded & addition_vk_loaded &
        transfer_pk_loaded & transfer_vk_loaded &
        withdrawal_pk_loaded & withdrawal_vk_loaded;

    Json::Value result;
    result["ready"] = ready;
    return result;
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
std::string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::unpack256Bits(
    const std::string& param01,
    const std::string& param02
    ) {
    vector<FieldT> elements{FieldT(param01.c_str()), FieldT(param02.c_str())};
    return bits2hex(unpack(elements));
}

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
bool zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::verifyProof(
    const std::string &proof_type,
    const Json::Value &proof_json,
    const Json::Value &public_inputs_json)
{
    vector<FieldT> public_inputs(public_inputs_json.size());
    for (uint i = 0; i < public_inputs_json.size(); i++)
    {
        public_inputs[i] = FieldT(public_inputs_json[i].asString().c_str());
    }
    auto proof = json_conversion::json_to_proof_jacobian(proof_json);

    r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> vk;
    if (!strcmp(proof_type.c_str(), "commitment"))
    {
        vk = commitment_vk;
    }
    else if (!strcmp(proof_type.c_str(), "addition"))
    {
        vk = addition_vk;
    }
    else if (!strcmp(proof_type.c_str(), "transfer"))
    {
        vk = transfer_vk;
    }
    else if (!strcmp(proof_type.c_str(), "withdrawal"))
    {
        vk = withdrawal_vk;
    }
    else if (!strcmp(proof_type.c_str(), "example"))
    {
        vk = example_vk;
    }
    else
    {
        throw JsonRpcException(-32602, "Invalid proof type");
    }

    //TODO: rename vars or extract func
    auto start = std::chrono::system_clock::now();
    std::time_t readableStart = std::chrono::system_clock::to_time_t(start);
    cout << "Called at: " << std::ctime(&readableStart) << endl;
    cout << "PRIMARY INPUT" << endl << dec << public_inputs << endl;
    bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
        vk,
        public_inputs,
        proof);
    if (verified)
    {
        cout << "Proof verified!" << endl;
    }
    else
    {
        cout << "Proof NOT verified :(" << endl;
    }
    return verified;
}
