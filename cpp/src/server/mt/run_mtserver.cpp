#include <jsonrpccpp/server/connectors/httpserver.h>
#include "definitions.hpp"
#include "MTServer.hpp"
#include "circuitry/gadgets/dummyhash_gadget.hpp"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Need exactly 1 argument (tree height)" << std::endl;
    return 1;
  }

  default_r1cs_ppzksnark_pp::init_public_params();

  HttpServer httpserver(5000);
  MTServer<FieldT, MerkleTreeHashT> mtserver(std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);

  mtserver.StartListening();

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
