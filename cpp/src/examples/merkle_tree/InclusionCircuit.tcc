template<typename FieldT, typename HashT>
InclusionCircuit<FieldT, HashT> make_inclusion_circuit(size_t tree_height) {
  protoboard<FieldT>* pb = new protoboard<FieldT>();

  pb_variable_array<FieldT>* root_va = new pb_variable_array<FieldT>();
  root_va->allocate(*pb, 2, "root_va");
 
  digest_variable<FieldT>* root_digest = new digest_variable<FieldT>(*pb, HashT::get_digest_len(), "root_digest");

  multipacking_gadget<FieldT>* root_unpacker = new multipacking_gadget<FieldT>(
    *pb,
    root_digest->bits,
    *root_va,
    128,
    "root_unpacker"
  );

  pb_variable_array<FieldT>* address_bits_va = new pb_variable_array<FieldT>();
  address_bits_va->allocate(*pb, tree_height, "address_bits_va");

  digest_variable<FieldT>* leaf_digest = new digest_variable<FieldT>(*pb, HashT::get_digest_len(), "leaf_digest");

  merkle_authentication_path_variable<FieldT, HashT>* path_var = new merkle_authentication_path_variable<FieldT, HashT>(*pb, tree_height, "merkle_authentication_path");

  merkle_tree_check_read_gadget<FieldT, HashT>* mtcrg = new merkle_tree_check_read_gadget<FieldT, HashT>(
    *pb,
    tree_height,
    *address_bits_va,
    *leaf_digest,
    *root_digest,
    *path_var,
    ONE,
    "merkle_tree_check_read_gadget"
  );

  pb->set_input_sizes(2);
  root_digest->generate_r1cs_constraints();
  root_unpacker->generate_r1cs_constraints(true);
  leaf_digest->generate_r1cs_constraints();
  path_var->generate_r1cs_constraints();
  mtcrg->generate_r1cs_constraints();

  InclusionCircuit<FieldT, HashT> circuit{
    pb,
    root_va,
    root_digest,
    root_unpacker,
    address_bits_va,
    leaf_digest,
    path_var,
    mtcrg
  };

  return circuit;
}