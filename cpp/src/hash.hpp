#ifndef ZKTRADE_HASH_HPP
#define ZKTRADE_HASH_HPP

#include "definitions.hpp"
#include "printbits.hpp"
#include "serialization.cpp"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "Need 2 hex values" << endl;
    return 1;
  }
  default_r1cs_ppzksnark_pp::init_public_params();
  const libff::bit_vector left = hex2bits(argv[1]);
  const libff::bit_vector right = hex2bits(argv[2]);
  libff::bit_vector block = left;
  block.insert(block.end(), right.begin(), right.end());
  libff::bit_vector hash = MerkleTreeHashT::get_hash(block);
  printbits(hash);
}

#endif //ZKTRADE_HASH_HPP
