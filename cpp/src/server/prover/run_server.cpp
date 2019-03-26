#include <jsonrpccpp/server/connectors/httpserver.h>
#include "definitions.hpp"
#include "Server.hpp"
#include "circuitry/gadgets/dummyhash_gadget.hpp"

int main(int argc, char* argv[])
{
  if (argc != 11) {
    std::cerr << "Need exactly 9 arguments (tree height, paths to commitment ";
    std::cerr << "pk, commitment vk, addition pk, addition vk, transfer pk, ";
    std::cerr << "transfer vk, withdrawal pk, withdrawal vk, port)";
    std::cerr << std::endl;
    return 1;
  }

  default_r1cs_ppzksnark_pp::init_public_params();

  HttpServer httpserver(atoi(argv[10]));
  Server<FieldT, CommitmentHashT, MerkleTreeHashT> server(std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);

  server.StartListening();

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

  std::cout << "Server started. Enter \"end\" to shut down." << std::endl;

  bool need_to_shut_down = false;
  while(!need_to_shut_down) {
    std::string x;
    std::cin >> x;
    if (x == "") {
      // somehow there is constantly some input on circleci, ignore this
    } else if (x == "end") {
      need_to_shut_down = true;
    } else {
      std::cout << "You entered \"" << x << "\". Enter \"end\" to shut down." << std::endl;
    }
  }

  return 0;
}
