#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include "../../verify.cpp"

typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

int main(int argc, char* argv[]) {
  if (argc != 10) {
    cerr << "Need exactly 9 arguments (path to vk, path to proof, address, prev root part 1, prev root part 2, leaf part 1, leaf part 2, next root part 1, next root part 2)" << endl;
    return 1;
  }

  default_r1cs_ppzksnark_pp::init_public_params();

  std::vector<FieldT> field_elements;

  for (uint i = 0; i < 7; i++) {
    cout << i << endl;
    cout << "input " << argv[i + 3] << endl;
    cout << "felem " << FieldT(argv[i + 3]) << endl;
    field_elements.push_back(FieldT(argv[i + 3]));
  }

  cout << "Field elems" << endl << field_elements << endl;

  return verify(argv[1], argv[2], field_elements);
}
