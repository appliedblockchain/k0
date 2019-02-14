template <typename FieldT, typename HashT>
InclusionCircuit<FieldT, HashT> make_inclusion_circuit(size_t tree_height)
{
  protoboard<FieldT>*pb = new protoboard<FieldT>();

  pb_variable_array<FieldT>* root_va = new pb_variable_array<FieldT>();
  root_va->allocate(*pb, 2, "root_va");

  pb_variable_array<FieldT>* sn_va = new pb_variable_array<FieldT>();
  sn_va->allocate(*pb, 2, "sn_packed");

  digest_variable<FieldT>* root_digest = new digest_variable<FieldT>(
     *pb, HashT::get_digest_len(), "root_digest");

  multipacking_gadget<FieldT>* root_unpacker = new multipacking_gadget<FieldT>(
     *pb,
      root_digest->bits,
      *root_va,
      128,
      "root_unpacker");

  digest_variable<FieldT>* sn_digest = new digest_variable<FieldT>(
      *pb, HashT::get_digest_len(), "sn_digest");
  multipacking_gadget<FieldT>* sn_unpacker = new multipacking_gadget<FieldT>(
      *pb,
      sn_digest->bits,
      *sn_va,
      128,
      "sn_unpacker");

  digest_variable<FieldT>* r_digest = new digest_variable<FieldT>(
      *pb, HashT::get_digest_len(), "r_digest");

  digest_variable<FieldT>* leaf_digest = new digest_variable<FieldT>(
      *pb, HashT::get_digest_len(), "leaf_digest");

  sha256_two_to_one_hash_gadget<FieldT>* leaf_hasher = new sha256_two_to_one_hash_gadget<FieldT>(
      *pb, *r_digest, *sn_digest, *leaf_digest, "leaf_hasher");

  pb_variable_array<FieldT>* address_bits_va = new pb_variable_array<FieldT>();
  address_bits_va->allocate(*pb, tree_height, "address_bits_va");

  merkle_authentication_path_variable<FieldT, HashT>* path_var = new merkle_authentication_path_variable<FieldT, HashT>(*pb, tree_height, "merkle_authentication_path");

  merkle_tree_check_read_gadget<FieldT, HashT>* mtcrg = new merkle_tree_check_read_gadget<FieldT, HashT>(
      *pb,
      tree_height,
      *address_bits_va,
      *leaf_digest,
      *root_digest,
      *path_var,
      ONE,
      "merkle_tree_check_read_gadget");

  pb->set_input_sizes(4);
  root_digest->generate_r1cs_constraints();
  root_unpacker->generate_r1cs_constraints(true);
  sn_digest->generate_r1cs_constraints();
  sn_unpacker->generate_r1cs_constraints(true);
  r_digest->generate_r1cs_constraints();
  leaf_digest->generate_r1cs_constraints();
  leaf_hasher->generate_r1cs_constraints(true);
  path_var->generate_r1cs_constraints();
  mtcrg->generate_r1cs_constraints();

  InclusionCircuit<FieldT, HashT> circuit{
      pb,
      root_va,
      root_digest,
      root_unpacker,
      sn_va,
      sn_digest,
      sn_unpacker,
      r_digest,
      leaf_digest,
      leaf_hasher,
      address_bits_va,
      path_var,
      mtcrg};

  return circuit;
}