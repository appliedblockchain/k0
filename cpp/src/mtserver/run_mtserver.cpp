#include <jsonrpccpp/server/connectors/httpserver.h>
#include "definitions.hpp"
#include "MTServer.hpp"
#include "circuitry/gadgets/dummyhash_gadget.hpp"

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << "Need exactly 2 arguments (tree height, port)" << std::endl;
    return 1;
  }

  default_r1cs_ppzksnark_pp::init_public_params();

  HttpServer httpserver(atoi(argv[2]));
  MTServer<FieldT, MerkleTreeHashT> mtserver(std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);

  mtserver.StartListening();

  std::cout << "Server started." << std::endl;
  cout << "Hit enter to stop the server." << endl;
  getchar();

  mtserver.StopListening();

  return 0;
}
