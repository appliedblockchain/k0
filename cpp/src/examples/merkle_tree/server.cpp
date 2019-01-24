#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include "MerkleTreeServer.hpp"

int main(int argc, char* argv[])
{
  if (argc != 6) {
    std::cerr << "Need exactly 5 arguments (tree height, path to addition pk, path to addition vk, path to inclusion pk, path to inclusion vk)" << std::endl;
    return 1;
  }

  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
  typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
  default_r1cs_ppzksnark_pp::init_public_params();

  HttpServer httpserver(4000);
  MerkleTreeServer<FieldT, HashT> server(std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);

  server.StartListening();

  std::cout << "Loading addition proving key..." << std::endl;
  server.setAdditionPk(argv[2]);
  std::cout << "Loading addition verification key..." << std::endl;
  server.setAdditionVk(argv[3]);

  std::cout << "Loading inclusion proving key..." << std::endl;
  server.setInclusionPk(argv[4]);
  std::cout << "Loading inclusion verification key..." << std::endl;
  server.setInclusionVk(argv[5]);

  std::cout << "Server started. Press a button to stop." << std::endl;
  getchar();
  server.StopListening();
  return 0;
}
