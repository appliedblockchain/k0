#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include "./serialization.cpp"

using namespace libsnark;

typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Need hex string" << endl;
    return 1;
  }
  default_r1cs_ppzksnark_pp::init_public_params();
  protoboard<FieldT> pb;
  pb_variable_array<FieldT> packed;
  packed.allocate(pb, 2, "packed");
  pb_variable_array<FieldT> bits;
  bits.allocate(pb, 256, "bits");
  multipacking_gadget<FieldT> packer(pb, bits, packed, 128, "packer");
  packer.generate_r1cs_constraints(false);
  pb.set_input_sizes(2);
  const libff::bit_vector bitv = hex2bits(argv[1]);
  bits.fill_with_bits(pb, bitv);
  packer.generate_r1cs_witness_from_bits();
  assert(pb.is_satisfied());
  std::cout << pb.primary_input()[0] << "," << pb.primary_input()[1] << std::endl;
  return 0;
}