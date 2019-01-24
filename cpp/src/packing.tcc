template<typename FieldT>
vector<FieldT> pack(bit_vector bv)
{
  protoboard<FieldT> pb;
  pb_variable_array<FieldT> packed;
  packed.allocate(pb, 2, "packed");
  pb_variable_array<FieldT> bits;
  bits.allocate(pb, 256, "bits");
  multipacking_gadget<FieldT> packer(pb, bits, packed, 128, "packer");
  packer.generate_r1cs_constraints(false);
  pb.set_input_sizes(2);
  bits.fill_with_bits(pb, bv);
  packer.generate_r1cs_witness_from_bits();
  assert(pb.is_satisfied());
  return pb.primary_input();
}

template<typename FieldT>
bit_vector unpack(vector<FieldT> fev)
{
  protoboard<FieldT> pb;
  pb_variable_array<FieldT> bits;
  bits.allocate(pb, 256, "bits");
  pb_variable_array<FieldT> packed;
  packed.allocate(pb, 2, "packed");
  multipacking_gadget<FieldT> packer(pb, bits, packed, 128, "packer");
  packer.generate_r1cs_constraints(false);
  pb.set_input_sizes(256);
  packed.fill_with_field_elements(pb, fev);
  packer.generate_r1cs_witness_from_packed();
  assert(pb.is_satisfied());
  bit_vector result;
  for (uint i = 0; i < bits.size(); i++) {
    result.push_back(pb.val(bits[i]) == FieldT::zero() ? false : true);
  }
  return result;
}