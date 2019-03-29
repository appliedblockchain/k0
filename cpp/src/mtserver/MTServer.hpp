#pragma once

#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "mtstubserver.h"
#include "MerkleTree.hpp"

using namespace jsonrpc;
using namespace libsnark;

namespace zktrade {

    template<typename FieldT, typename MerkleTreeHashT>
    class MTServer : public MTStubServer {
        size_t tree_height;
        MerkleTree<MerkleTreeHashT> mt;
    public:
        MTServer(size_t tree_height, AbstractServerConnector &connector,
               serverVersion_t type);

        virtual Json::Value add(const std::string &param01);

        virtual std::string element(int param01);

        virtual Json::Value path(const std::string &address_dec);

        virtual std::string reset();

        virtual std::string root();

        virtual Json::Value simulateAddition(const std::string& param01);

        virtual Json::Value status();
    };

}

#include "MTServer.tcc"
