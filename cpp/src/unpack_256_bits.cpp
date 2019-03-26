#include "definitions.hpp"
#include "packing.hpp"
#include "serialization.hpp"

int main(int argc, char* argv[])
{
  if (argc != 3) {
    cerr << "Need two values" << endl;
    return 1;
  }
  default_r1cs_ppzksnark_pp::init_public_params();
  vector<FieldT> elements{FieldT(argv[1]), FieldT(argv[2])};
  std::cout << bits2hex(unpack(elements)) << endl;
  return 0;
}