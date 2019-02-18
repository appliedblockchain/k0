template<typename FieldT, typename HashT>
MTLeafAddition<FieldT, HashT>::MTLeafAddition(protoboard<FieldT> &pb,
                                              const size_t tree_height,
                                              const pb_variable_array<FieldT> &address_bits_va,
                                              const digest_variable<FieldT> &prev_root_digest,
                                              const digest_variable<FieldT> &leaf_digest,
                                              const digest_variable<FieldT> &next_root_digest,
                                              const merkle_authentication_path_variable<FieldT, HashT> &path_var)
        :
        gadget<FieldT>(pb, "mt_leaf_addition"),
        address_bits_va(address_bits_va),
        prev_leaf_digest(pb, HashT::get_digest_len(), "prev_leaf"),
        prev_root_digest(prev_root_digest),
        next_leaf_digest(leaf_digest),
        next_root_digest(next_root_digest),
        prev_path_var(path_var),
        next_path_var(pb, tree_height, "next_path"),
        mtcug(pb, tree_height, address_bits_va,
              prev_leaf_digest, prev_root_digest, prev_path_var,
              next_leaf_digest, next_root_digest, next_path_var, ONE, "mtcug") {
    ZERO.allocate(this->pb, "zero");
}

template<typename FieldT, typename HashT>
void MTLeafAddition<FieldT, HashT>::generate_r1cs_constraints() {
    // We're only adding elements, therefore the previous leaf always needs to be zero
    const size_t digest_len = HashT::get_digest_len();
    for (int i = 0; i < digest_len; i++) {
        this->pb.add_r1cs_constraint(
                r1cs_constraint<FieldT>(prev_leaf_digest.bits[i], ONE, ZERO),
                "bit " + std::to_string(i) + " needs to be zero");
    }

    mtcug.generate_r1cs_constraints();
}

template<typename FieldT, typename HashT>
void MTLeafAddition<FieldT, HashT>::generate_r1cs_witness() {
    this->pb.val(ZERO) = FieldT("0");
    const size_t digest_len = HashT::get_digest_len();
    for (int i = 0; i < digest_len; i++) {
        this->pb.val(prev_leaf_digest.bits[i]) = FieldT("0");
    }
    mtcug.generate_r1cs_witness();
}
