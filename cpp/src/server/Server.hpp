#pragma once

#include "k0stubserver.h"
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "MerkleTree.hpp"

using namespace jsonrpc;
using namespace libsnark;

namespace k0 {
    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    class Server : public K0StubServer {
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> commitment_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> commitment_vk;
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> addition_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> addition_vk;
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> transfer_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> transfer_vk;
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> withdrawal_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> withdrawal_vk;
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> example_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> example_vk;
        size_t tree_height;
        MerkleTree<MerkleTreeHashT> mt;
        bool commitment_pk_loaded;
        bool commitment_vk_loaded;
        bool addition_pk_loaded;
        bool addition_vk_loaded;
        bool transfer_pk_loaded;
        bool transfer_vk_loaded;
        bool withdrawal_pk_loaded;
        bool withdrawal_vk_loaded;
        bool example_pk_loaded;
        bool example_vk_loaded;
    public:
        Server(size_t tree_height, AbstractServerConnector &connector,
               serverVersion_t type);

        void setCommitmentPk(std::string pk_path);

        void setCommitmentVk(std::string vk_path);

        void setAdditionPk(std::string pk_path);

        void setAdditionVk(std::string vk_path);

        void setTransferPk(std::string pk_path);

        void setTransferVk(std::string vk_path);

        void setWithdrawalPk(std::string pk_path);

        void setWithdrawalVk(std::string vk_path);

        void setExamplePk(std::string pk_path);

        void setExampleVk(std::string vk_path);

        virtual Json::Value add(const std::string &param01);

        virtual std::string
        cm(const std::string &param01, const std::string &param02,
           const std::string &param03, const std::string &param04);

        virtual Json::Value decrypt_note(const std::string& ciphertext,
                                         const std::string& sk_enc,
                                         const std::string& pk_enc);

        virtual Json::Value deriveKeys(const std::string& a_sk);

        virtual std::string element(int param01);

        virtual std::string encrypt_note(const std::string& plaintext,
                                         const std::string& pk_enc);

        virtual Json::Value
        exampleWitnessAndProof(const std::string &x);

        virtual std::string
        hash(const std::string &param01, const std::string &param02);

        virtual Json::Value
        depositCommitmentProof(const std::string &a_pk_str,
                               const std::string &rho_str,
                               const std::string &r_str,
                               const std::string &v_str);

        virtual Json::Value
        merkleTreeAdditionProof(const std::string &prev_root_hex,
                                const std::string &address_dec,
                                const std::string &leaf_hex,
                                const Json::Value &path,
                                const std::string &next_root_hex);

        virtual Json::Value
        prepareTransfer(const std::string &prev_root_hex,
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
                        const std::string &callee_hex_str);

        virtual Json::Value
        prepare_withdrawal(const std::string &address_str,
                           const std::string &a_sk_str,
                           const std::string &rho_str,
                           const std::string &r_str,
                           const std::string &v_str,
                           const std::string &recipient_str);

        virtual Json::Value pack256Bits(const std::string& param01);

        virtual std::string prf_addr(const std::string &a_sk);

        virtual std::string reset();

        virtual std::string root();

        virtual Json::Value status();

        virtual std::string unpack256Bits(const std::string& param01, const std::string& param02);

        virtual bool verifyProof(const std::string& proofType,
        const Json::Value& proof,
        const Json::Value& public_inputs);
    };

}

#include "Server.tcc"
