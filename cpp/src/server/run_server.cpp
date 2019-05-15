#include "Server.hpp"
#include "circuitry/gadgets/dummyhash_gadget.hpp"
#include "definitions.hpp"
#include <jsonrpccpp/server/connectors/httpserver.h>

int main(int argc, char *argv[]) {
    cout << "argc " << argc << endl;
    cout << argv[0] << endl;
    if (argc != 13) {
        std::cerr
            << "Need exactly 12 arguments (tree height, paths to commitment ";
        std::cerr
            << "pk, commitment vk, addition pk, addition vk, transfer pk, ";
        std::cerr << "transfer vk, withdrawal pk, withdrawal vk, example pk, ";
        std::cerr << "example vk, port)";
        std::cerr << std::endl;
        return 1;
    }

    default_r1cs_ppzksnark_pp::init_public_params();

    auto port = atoi(argv[12]);

    HttpServer httpserver(port);
    Server<FieldT, CommitmentHashT, MerkleTreeHashT> server(
        std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);


    std::cout << "Loading commitment proving key..." << std::endl;
    server.setCommitmentPk(argv[2]);
    std::cout << "Loading commitment verification key..." << std::endl;
    server.setCommitmentVk(argv[3]);

    std::cout << "Loading addition proving key..." << std::endl;
    server.setAdditionPk(argv[4]);
    std::cout << "Loading addition verification key..." << std::endl;
    server.setAdditionVk(argv[5]);

    std::cout << "Loading transfer proving key..." << std::endl;
    server.setTransferPk(argv[6]);
    std::cout << "Loading transfer verification key..." << std::endl;
    server.setTransferVk(argv[7]);

    std::cout << "Loading withdrawal proving key..." << std::endl;
    server.setWithdrawalPk(argv[8]);
    std::cout << "Loading withdrawal verification key..." << std::endl;
    server.setWithdrawalVk(argv[9]);

    std::cout << "Loading example proving key..." << std::endl;
    server.setExamplePk(argv[10]);
    std::cout << "Loading example verification key..." << std::endl;
    server.setExampleVk(argv[11]);

    server.StartListening();

    std::cout << "Server started (port " << port << "). ";

    cout << "Hit enter to stop the server." << endl;
    getchar();

    server.StopListening();

    return 0;
}
