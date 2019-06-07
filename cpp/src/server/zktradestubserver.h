/**
 * This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
 */

#ifndef JSONRPC_CPP_STUB_ZKTRADESTUBSERVER_H_
#define JSONRPC_CPP_STUB_ZKTRADESTUBSERVER_H_

#include <jsonrpccpp/server.h>

class ZKTradeStubServer : public jsonrpc::AbstractServer<ZKTradeStubServer>
{
    public:
        ZKTradeStubServer(jsonrpc::AbstractServerConnector &conn, jsonrpc::serverVersion_t type = jsonrpc::JSONRPC_SERVER_V2) : jsonrpc::AbstractServer<ZKTradeStubServer>(conn, type)
        {
            this->bindAndAddMethod(jsonrpc::Procedure("add", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::addI);
            this->bindAndAddMethod(jsonrpc::Procedure("cm", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING,"param03",jsonrpc::JSON_STRING,"param04",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::cmI);
            this->bindAndAddMethod(jsonrpc::Procedure("decrypt_note", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING,"param03",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::decrypt_noteI);
            this->bindAndAddMethod(jsonrpc::Procedure("deriveKeys", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::deriveKeysI);
            this->bindAndAddMethod(jsonrpc::Procedure("element", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param01",jsonrpc::JSON_INTEGER, NULL), &ZKTradeStubServer::elementI);
            this->bindAndAddMethod(jsonrpc::Procedure("encrypt_note", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::encrypt_noteI);
            this->bindAndAddMethod(jsonrpc::Procedure("exampleWitnessAndProof", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::exampleWitnessAndProofI);
            this->bindAndAddMethod(jsonrpc::Procedure("hash", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::hashI);
            this->bindAndAddMethod(jsonrpc::Procedure("depositCommitmentProof", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING,"param03",jsonrpc::JSON_STRING,"param04",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::depositCommitmentProofI);
            this->bindAndAddMethod(jsonrpc::Procedure("merkleTreeAdditionProof", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_ARRAY, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING,"param03",jsonrpc::JSON_STRING,"param04",jsonrpc::JSON_ARRAY,"param05",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::merkleTreeAdditionProofI);
            this->bindAndAddMethod(jsonrpc::Procedure("prepareTransfer", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING,"param03",jsonrpc::JSON_STRING,"param04",jsonrpc::JSON_STRING,"param05",jsonrpc::JSON_STRING,"param06",jsonrpc::JSON_STRING,"param07",jsonrpc::JSON_ARRAY,"param08",jsonrpc::JSON_STRING,"param09",jsonrpc::JSON_STRING,"param10",jsonrpc::JSON_STRING,"param11",jsonrpc::JSON_STRING,"param12",jsonrpc::JSON_STRING,"param13",jsonrpc::JSON_ARRAY,"param14",jsonrpc::JSON_STRING,"param15",jsonrpc::JSON_STRING,"param16",jsonrpc::JSON_STRING,"param17",jsonrpc::JSON_STRING,"param18",jsonrpc::JSON_STRING,"param19",jsonrpc::JSON_STRING,"param20",jsonrpc::JSON_STRING,"param21",jsonrpc::JSON_STRING,"param22",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::prepareTransferI);
            this->bindAndAddMethod(jsonrpc::Procedure("prepare_withdrawal", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING,"param03",jsonrpc::JSON_STRING,"param04",jsonrpc::JSON_STRING,"param05",jsonrpc::JSON_STRING,"param06",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::prepare_withdrawalI);
            this->bindAndAddMethod(jsonrpc::Procedure("pack256Bits", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_ARRAY, "param01",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::pack256BitsI);
            this->bindAndAddMethod(jsonrpc::Procedure("prf_addr", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param01",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::prf_addrI);
            this->bindAndAddMethod(jsonrpc::Procedure("reset", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING,  NULL), &ZKTradeStubServer::resetI);
            this->bindAndAddMethod(jsonrpc::Procedure("root", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING,  NULL), &ZKTradeStubServer::rootI);
            this->bindAndAddMethod(jsonrpc::Procedure("status", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT,  NULL), &ZKTradeStubServer::statusI);
            this->bindAndAddMethod(jsonrpc::Procedure("unpack256Bits", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_STRING, NULL), &ZKTradeStubServer::unpack256BitsI);
            this->bindAndAddMethod(jsonrpc::Procedure("verifyProof", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_BOOLEAN, "param01",jsonrpc::JSON_STRING,"param02",jsonrpc::JSON_OBJECT,"param03",jsonrpc::JSON_ARRAY, NULL), &ZKTradeStubServer::verifyProofI);
        }

        inline virtual void addI(const Json::Value &request, Json::Value &response)
        {
            response = this->add(request[0u].asString());
        }
        inline virtual void cmI(const Json::Value &request, Json::Value &response)
        {
            response = this->cm(request[0u].asString(), request[1u].asString(), request[2u].asString(), request[3u].asString());
        }
        inline virtual void decrypt_noteI(const Json::Value &request, Json::Value &response)
        {
            response = this->decrypt_note(request[0u].asString(), request[1u].asString(), request[2u].asString());
        }
        inline virtual void deriveKeysI(const Json::Value &request, Json::Value &response)
        {
            response = this->deriveKeys(request[0u].asString());
        }
        inline virtual void elementI(const Json::Value &request, Json::Value &response)
        {
            response = this->element(request[0u].asInt());
        }
        inline virtual void encrypt_noteI(const Json::Value &request, Json::Value &response)
        {
            response = this->encrypt_note(request[0u].asString(), request[1u].asString());
        }
        inline virtual void exampleWitnessAndProofI(const Json::Value &request, Json::Value &response)
        {
            response = this->exampleWitnessAndProof(request[0u].asString());
        }
        inline virtual void hashI(const Json::Value &request, Json::Value &response)
        {
            response = this->hash(request[0u].asString(), request[1u].asString());
        }
        inline virtual void depositCommitmentProofI(const Json::Value &request, Json::Value &response)
        {
            response = this->depositCommitmentProof(request[0u].asString(), request[1u].asString(), request[2u].asString(), request[3u].asString());
        }
        inline virtual void merkleTreeAdditionProofI(const Json::Value &request, Json::Value &response)
        {
            response = this->merkleTreeAdditionProof(request[0u].asString(), request[1u].asString(), request[2u].asString(), request[3u], request[4u].asString());
        }
        inline virtual void prepareTransferI(const Json::Value &request, Json::Value &response)
        {
            response = this->prepareTransfer(request[0u].asString(), request[1u].asString(), request[2u].asString(), request[3u].asString(), request[4u].asString(), request[5u].asString(), request[6u], request[7u].asString(), request[8u].asString(), request[9u].asString(), request[10u].asString(), request[11u].asString(), request[12u], request[13u].asString(), request[14u].asString(), request[15u].asString(), request[16u].asString(), request[17u].asString(), request[18u].asString(), request[19u].asString(), request[20u].asString(), request[21u].asString());
        }
        inline virtual void prepare_withdrawalI(const Json::Value &request, Json::Value &response)
        {
            response = this->prepare_withdrawal(request[0u].asString(), request[1u].asString(), request[2u].asString(), request[3u].asString(), request[4u].asString(), request[5u].asString());
        }
        inline virtual void pack256BitsI(const Json::Value &request, Json::Value &response)
        {
            response = this->pack256Bits(request[0u].asString());
        }
        inline virtual void prf_addrI(const Json::Value &request, Json::Value &response)
        {
            response = this->prf_addr(request[0u].asString());
        }
        inline virtual void resetI(const Json::Value &/*request*/, Json::Value &response)
        {
            response = this->reset();
        }
        inline virtual void rootI(const Json::Value &/*request*/, Json::Value &response)
        {
            response = this->root();
        }
        inline virtual void statusI(const Json::Value &/*request*/, Json::Value &response)
        {
            response = this->status();
        }
        inline virtual void unpack256BitsI(const Json::Value &request, Json::Value &response)
        {
            response = this->unpack256Bits(request[0u].asString(), request[1u].asString());
        }
        inline virtual void verifyProofI(const Json::Value &request, Json::Value &response)
        {
            response = this->verifyProof(request[0u].asString(), request[1u], request[2u]);
        }
        virtual Json::Value add(const std::string& param01) = 0;
        virtual std::string cm(const std::string& param01, const std::string& param02, const std::string& param03, const std::string& param04) = 0;
        virtual Json::Value decrypt_note(const std::string& param01, const std::string& param02, const std::string& param03) = 0;
        virtual Json::Value deriveKeys(const std::string& param01) = 0;
        virtual std::string element(int param01) = 0;
        virtual std::string encrypt_note(const std::string& param01, const std::string& param02) = 0;
        virtual Json::Value exampleWitnessAndProof(const std::string& param01) = 0;
        virtual std::string hash(const std::string& param01, const std::string& param02) = 0;
        virtual Json::Value depositCommitmentProof(const std::string& param01, const std::string& param02, const std::string& param03, const std::string& param04) = 0;
        virtual Json::Value merkleTreeAdditionProof(const std::string& param01, const std::string& param02, const std::string& param03, const Json::Value& param04, const std::string& param05) = 0;
        virtual Json::Value prepareTransfer(const std::string& param01, const std::string& param02, const std::string& param03, const std::string& param04, const std::string& param05, const std::string& param06, const Json::Value& param07, const std::string& param08, const std::string& param09, const std::string& param10, const std::string& param11, const std::string& param12, const Json::Value& param13, const std::string& param14, const std::string& param15, const std::string& param16, const std::string& param17, const std::string& param18, const std::string& param19, const std::string& param20, const std::string& param21, const std::string& param22) = 0;
        virtual Json::Value prepare_withdrawal(const std::string& param01, const std::string& param02, const std::string& param03, const std::string& param04, const std::string& param05, const std::string& param06) = 0;
        virtual Json::Value pack256Bits(const std::string& param01) = 0;
        virtual std::string prf_addr(const std::string& param01) = 0;
        virtual std::string reset() = 0;
        virtual std::string root() = 0;
        virtual Json::Value status() = 0;
        virtual std::string unpack256Bits(const std::string& param01, const std::string& param02) = 0;
        virtual bool verifyProof(const std::string& param01, const Json::Value& param02, const Json::Value& param03) = 0;
};

#endif //JSONRPC_CPP_STUB_ZKTRADESTUBSERVER_H_
