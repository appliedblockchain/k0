#include <chrono>
#include <thread>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include "circuitry/gadgets/dummyhash_gadget.hpp"
#include "definitions.hpp"
#include "MTServer.hpp"

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << "Need exactly 2 arguments (tree height, port)" << std::endl;
    return 1;
  }

  default_r1cs_ppzksnark_pp::init_public_params();

  auto port = atoi(argv[2]);

  HttpServer httpserver(port);
  MTServer<FieldT, MerkleTreeHashT> mtserver(std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);

  mtserver.StartListening();

  std::cout << "Server started (port " << port << ")." << std::endl;

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
      std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  mtserver.StopListening();

  return 0;
}
