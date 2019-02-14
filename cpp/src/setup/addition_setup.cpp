#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "../circuitry/AdditionCircuit.hpp"
#include "../util.h"
#include "setup.cpp"

using namespace libff;
using namespace libsnark;
using namespace std;

int main(int argc, char* argv[])
{
  if (argc != 4) {
    std::cerr << "Need exactly 3 arguments (tree height, path to pk, path to vk)" << std::endl;
    return 1;
  }

  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
  typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
  default_r1cs_ppzksnark_pp::init_public_params();

  size_t tree_height = stoi(argv[1]);

  cout << endl << "Tree height " << tree_height << endl;
  AdditionCircuit<FieldT, HashT> circuit = make_addition_circuit<FieldT, HashT>(tree_height);
  setup(circuit.pb->get_constraint_system(), argv[2], argv[3]);
}

