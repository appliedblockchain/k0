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

template<typename FieldT, typename HashT>
zktrade::Server<FieldT, HashT>::Server(size_t height,
                                       AbstractServerConnector &connector,
                                       serverVersion_t type)
        : ZKTradeStubServer(connector, type),
          tree_height{height},
          mt{height},
          deposit_circuit{make_deposit_circuit<FieldT>(height)},
          withdrawal_circuit{make_withdrawal_circuit<FieldT>(height)} {
}

template<typename FieldT, typename HashT>
string zktrade::Server<FieldT, HashT>::reset() {
    mt = MerkleTree{tree_height};
    return bits2hex(mt.root());
}

template<typename FieldT, typename HashT>
string zktrade::Server<FieldT, HashT>::root() {
    return bits2hex(mt.root());
}

template<typename FieldT, typename HashT>
void zktrade::Server<FieldT, HashT>::setDepositPk(string pk_path) {
    deposit_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
            pk_path);
    deposit_pk_loaded = true;
}

template<typename FieldT, typename HashT>
void zktrade::Server<FieldT, HashT>::setDepositVk(string vk_path) {
    deposit_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
            vk_path);
    deposit_vk_loaded = true;
}

template<typename FieldT, typename HashT>
void zktrade::Server<FieldT, HashT>::setWithdrawalPk(string pk_path) {
    withdrawal_pk = loadFromFile<r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp>>(
            pk_path);
    withdrawal_pk_loaded = true;
}

template<typename FieldT, typename HashT>
void zktrade::Server<FieldT, HashT>::setWithdrawalVk(string vk_path) {
    withdrawal_vk = loadFromFile<r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp>>(
            vk_path);
    withdrawal_vk_loaded = true;
}

template<typename FieldT, typename HashT>
Json::Value zktrade::Server<FieldT, HashT>::add(const string &leaf_hex) {
    bit_vector leaf_bv = hex2bits(leaf_hex);
    uint address = mt.add(leaf_bv);
    mt.print();
    Json::Value result;
    result["address"] = address;
    result["newRoot"] = bits2hex(mt.root());
    return result;
}

template<typename FieldT, typename HashT>
string zktrade::Server<FieldT, HashT>::element(int address) {
    if (mt.num_elements() == 0 || address > (mt.num_elements() - 1)) {
        throw JsonRpcException(-32602, "Address too big");
    }
    return bits2hex(mt[address]);
}

template<typename FieldT, typename HashT>
std::string zktrade::Server<FieldT, HashT>::hash(const std::string &left_hex,
                                                 const std::string &right_hex) {
    libff::bit_vector block = hex2bits(left_hex);
    libff::bit_vector right = hex2bits(right_hex);
    block.insert(block.end(), right.begin(), right.end());
    return bits2hex(HashT::get_hash(block));
}

template<typename FieldT, typename HashT>
Json::Value zktrade::Server<FieldT, HashT>::prepare_deposit(
        const std::string &a_pk_str,
        const std::string &rho_str,
        const std::string &r_str,
        const std::string &v_str) {
    bit_vector a_pk_bits = hex2bits(a_pk_str);
    bit_vector rho_bits = hex2bits(rho_str);
    bit_vector r_bits = hex2bits(r_str);
    FieldT v = FieldT(v_str.c_str());
    bit_vector v_bits = field_element_to_64_bits(v);

    auto k_bits = comm_r(a_pk_bits, rho_bits, r_bits);
    auto cm_bits = comm_s(k_bits, v_bits);


    auto sim_result = mt.simulate_add(cm_bits);

    uint address = mt.num_elements();

    auto dep_circuit = make_deposit_circuit<FieldT>(tree_height);
    cout << " root in server " << bits2hex(mt.root()) << endl;
    cout << " root in server elems " << dec << field_elements_from_bits<FieldT>(mt.root()) << endl;
    dep_circuit.prev_root_bits->generate_r1cs_witness(mt.root());

    dep_circuit.pb->val(*dep_circuit.address_packed) = address;
    dep_circuit.k_bits->fill_with_bits(*dep_circuit.pb, k_bits);
    dep_circuit.k_packer->generate_r1cs_witness_from_bits();
    dep_circuit.pb->val(*dep_circuit.v_packed) = v;
    dep_circuit.v_packer->generate_r1cs_witness_from_packed();

    dep_circuit.commitment_gadget->generate_r1cs_witness();

    dep_circuit.next_root_bits->generate_r1cs_witness(get<1>(sim_result));
    dep_circuit.path_var->generate_r1cs_witness(address, get<2>(sim_result));
    dep_circuit.mt_addition_gadget->generate_r1cs_witness();
    // ASSERT_FALSE(dep_circuit.pb->is_satisfied());

    // compare prev root
    assert(dep_circuit.prev_root_bits->get_digest() == mt.root());
    // compare next root
    assert(dep_circuit.next_root_bits->get_digest() == get<1>(sim_result));

    assert(dep_circuit.cm_bits->get_digest() == cm_bits);

    dep_circuit.cm_packer->generate_r1cs_witness_from_bits();
    dep_circuit.prev_root_packer->generate_r1cs_witness_from_bits();
    dep_circuit.next_root_packer->generate_r1cs_witness_from_bits();
    if (!dep_circuit.pb->is_satisfied()) {
        throw JsonRpcException(-32000, "Circuit not satisfied");
    }
//
//    cout << "Num inputs:    : " << deposit_pk.constraint_system.num_inputs() << endl;
//    cout << "Num variables  : " << deposit_pk.constraint_system.num_variables() << endl;
//    cout << "Num constraints: " << deposit_pk.constraint_system.num_constraints() << endl;
//
//    cout << "PRIMARY INPUT dec" << endl << dec << dep_circuit.pb->primary_input() << endl;
//    cout << "PRIMARY INPUT hex" << endl << hex << dep_circuit.pb->primary_input() << endl;

cout << "prev root used in proof on server " << dec << dep_circuit.pb->primary_input()[0];
    cout << " " << dep_circuit.pb->primary_input()[1] << endl;

    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof =
            r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(
                    deposit_pk, dep_circuit.pb->primary_input(),
                    dep_circuit.pb->auxiliary_input());
    bool verified =
            r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(
                    deposit_vk, dep_circuit.pb->primary_input(), proof);
    if (verified) {
        cout << "Successfully verified." << endl;
    } else {
        cerr << "Verification failed." << endl;
    }

    //writeToFile("/tmp/serverproof", proof);

    Json::Value result;
    result["k"] = bits_to_hex(k_bits);
    result["cm"] = bits_to_hex(cm_bits);
    result["nextRoot"] = bits_to_hex(get<1>(sim_result));

    Json::Value proof_in_json = json_conversion::to_json(proof);

    result["address"] = (uint)address;
    result["proof"] = proof_in_json;

    return result;
}

template<typename FieldT, typename HashT>
string zktrade::Server<FieldT, HashT>::prf_addr(const string& a_sk_hex)
{
    bit_vector a_sk = hex2bits(a_sk_hex);
    bit_vector a_pk = zktrade::prf_addr(a_sk);
    string a_pk_hex = bits2hex(a_pk);
    return a_pk_hex;
}

template<typename FieldT, typename HashT>
Json::Value zktrade::Server<FieldT, HashT>::status() {
    const bool ready =
            deposit_pk_loaded & deposit_vk_loaded & withdrawal_pk_loaded &
            withdrawal_vk_loaded;
    Json::Value result;
    result["ready"] = ready;
    return result;
}