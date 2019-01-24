template<typename FieldT, typename HashT>
MTLeafAdditionPacked<FieldT, HashT>::MTLeafAdditionPacked(protoboard<FieldT> &pb,
                                                          const size_t tree_height,
                                                          const pb_variable_array<FieldT> &address_bits_va,
                                                          const pb_variable_array<FieldT> &prev_root_va,
                                                          const pb_variable_array<FieldT> &leaf_va,
                                                          const pb_variable_array<FieldT> &next_root_va,
                                                          const merkle_authentication_path_variable<FieldT, HashT> &path_var) :
  gadget<FieldT>(pb, "packed_mt_leaf_addition"),
  prev_root_digest(pb, HashT::get_digest_len(), "prev_root_digest"),
// TODO calculate length (depending on field size and hash size)
  prev_root_unpacker(pb, prev_root_digest.bits, prev_root_va, 128 , "prev_root_unpacker"),
  leaf_digest(pb, HashT::get_digest_len(), "leaf_digest"),
  leaf_unpacker(pb, leaf_digest.bits, leaf_va, 128, "prev_leaf_unpacker"),
  next_root_digest(pb, HashT::get_digest_len(), "next_root_digest"),
  next_root_unpacker(pb, next_root_digest.bits, next_root_va, 128, "next_root_unpacker"),
  mtla(pb,
  tree_height,
  address_bits_va,
  prev_root_digest,
  leaf_digest,
  next_root_digest,
  path_var)
{
}

template<typename FieldT, typename HashT>
void MTLeafAdditionPacked<FieldT, HashT>::generate_r1cs_constraints()
{
  prev_root_unpacker.generate_r1cs_constraints(true);
  leaf_unpacker.generate_r1cs_constraints(true);
  next_root_unpacker.generate_r1cs_constraints(true);
  mtla.generate_r1cs_constraints();
}

template<typename FieldT, typename HashT>
void MTLeafAdditionPacked<FieldT, HashT>::generate_r1cs_witness()
{
  prev_root_unpacker.generate_r1cs_witness_from_packed();
  leaf_unpacker.generate_r1cs_witness_from_packed();
  next_root_unpacker.generate_r1cs_witness_from_packed();
  mtla.generate_r1cs_witness();
}
