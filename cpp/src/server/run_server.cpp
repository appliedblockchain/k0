#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include "Server.hpp"

int main(int argc, char* argv[])
{
  if (argc != 8) {
    std::cerr << "Need exactly 7 arguments (tree height, path to commitment ";
    std::cerr << "pk, path to commitment vk, path to addition pk, path to ";
    std::cerr << "addition vk, path to withdrawal pk, path to withdrawal vk)";
    std::cerr << std::endl;
    return 1;
  }

  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
  typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
  default_r1cs_ppzksnark_pp::init_public_params();

  HttpServer httpserver(4000);
  Server<FieldT, HashT> server(std::stoi(argv[1]), httpserver, jsonrpc::JSONRPC_SERVER_V2);

  server.StartListening();

  std::cout << "Loading commitment proving key..." << std::endl;
  server.setCommitmentPk(argv[2]);
  std::cout << "Loading commitment verification key..." << std::endl;
  server.setCommitmentVk(argv[3]);

  std::cout << "Loading addition proving key..." << std::endl;
  server.setAdditionPk(argv[4]);
  std::cout << "Loading addition verification key..." << std::endl;
  server.setAdditionVk(argv[5]);

  std::cout << "Loading withdrawal proving key..." << std::endl;
  server.setWithdrawalPk(argv[6]);
  std::cout << "Loading withdrawal verification key..." << std::endl;
  server.setWithdrawalVk(argv[7]);

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
