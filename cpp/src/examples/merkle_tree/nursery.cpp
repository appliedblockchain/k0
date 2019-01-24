#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include "../../printbits.hpp"
#include "../../serialization.cpp"
#include "../../MerkleTree.hpp"

#define TREE_HEIGHT 2

using namespace libff;
using namespace libsnark;
using namespace std;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_two_to_one_hash_gadget<FieldT> HashT;

// Because assert does nothing if NDEBUG is set
// (and I didn't find where it's set)
void myassert(bool thing) {
  if (thing) {
    cout << "Checked" << endl;
  } else {
    cout << "ERROR!" << endl;
    abort();
  }
}

bit_vector random_bits() {
  bit_vector v(256);
  generate(v.begin(), v.end(), [&]() { return rand() % 2; });
  return v;
}

int main(int argc, char* argv[]) {

  default_r1cs_ppzksnark_pp::init_public_params();

  MerkleTree mt(TREE_HEIGHT);
  mt.add(hex2bits(
    "0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
  ));
 
  mt.add(hex2bits(
    "0x2222222222222222222222222222222222222222222222222222222222222222"
  ));
 
  size_t address = 1;
  auto path = mt.path(address);
  cout << path.size() << endl;
  for (auto x : path) {
    printbits(x);
  }

  protoboard<FieldT> pb;
  merkle_authentication_path_variable<FieldT, HashT> path_var(pb, TREE_HEIGHT, "path_var");

  path_var.generate_r1cs_constraints();

  path_var.generate_r1cs_witness(address, path);

  auto inputs = pb.auxiliary_input();
  cout << inputs.size() << endl;
  for (uint i = 0; i < inputs.size(); i++) {
    cout << inputs[i];
    if (i % 100 == 0) {
      cout << endl;
    }
  }
  cout << endl;
  if (pb.is_satisfied()) {
    cout << "Satisfied" << endl;
  } else {
    cout << "Not Satisfied" << endl;
  }
  return 0;
}