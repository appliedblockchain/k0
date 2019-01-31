#include <chrono>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include "MerkleTreeServer.hpp"

void task1(std::string msg)
{
    std::cout << "task1 says: " << msg;
}

int main(int argc, char* argv[])
{

  auto start_timestamp = std::chrono::system_clock::now();
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

  auto init_done_timestamp = std::chrono::system_clock::now();
  auto init_duration = init_done_timestamp - start_timestamp;
  std::cout << "Init done. Elapsed time: " << init_duration.count() / 1000000 << "s";
  std::cout << std::endl;
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
