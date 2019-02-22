#pragma once

#include "zktradestubserver.h"
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "../MerkleTree.hpp"
#include "../circuitry/DepositCircuit.hpp"
#include "../circuitry/WithdrawalCircuit.hpp"

using namespace jsonrpc;
using namespace libsnark;

namespace zktrade {

    // TODO Remove HashT
    template<typename FieldT, typename HashT>
    class Server : public ZKTradeStubServer {
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> deposit_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> deposit_vk;
        r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> withdrawal_pk;
        r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> withdrawal_vk;
        size_t tree_height;
        MerkleTree mt;
        DepositCircuit<FieldT> deposit_circuit;
        WithdrawalCircuit<FieldT> withdrawal_circuit;
        bool deposit_pk_loaded;
        bool deposit_vk_loaded;
        bool withdrawal_pk_loaded;
        bool withdrawal_vk_loaded;
    public:
        Server(size_t tree_height, AbstractServerConnector &connector,
               serverVersion_t type);

        void setDepositPk(std::string pk_path);

        void setDepositVk(std::string pk_path);

        void setWithdrawalPk(std::string pk_path);

        void setWithdrawalVk(std::string pk_path);

        virtual Json::Value add(const std::string &param01);

        virtual std::string element(int param01);

        virtual std::string
        hash(const std::string &param01, const std::string &param02);

        virtual Json::Value prepare_deposit(
                const std::string &a_pk_str,
                const std::string &rho_str,
                const std::string &r_str,
                const std::string &v_str);

        virtual std::string prf_addr(const std::string& a_sk);

        virtual std::string reset();

        virtual std::string root();

        virtual Json::Value status();
    };

}

#include "Server.tcc"