template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
k0::input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>::input_note_gadget(
        const size_t &tree_height,
        protoboard<FieldT> &pb, pb_variable<FieldT> &ZERO,
        digest_variable<FieldT> &rt_bits, pb_variable_array<FieldT> &v_bits,
        pb_variable_array<FieldT> &a_sk_bits,
        pb_variable_array<FieldT> &rho_bits, pb_variable_array<FieldT> &r_bits,
        pb_variable_array<FieldT> &address_bits,
        merkle_authentication_path_variable<FieldT, MerkleTreeHashT> &path,
        shared_ptr<digest_variable<FieldT>> a_pk_bits,
        digest_variable<FieldT> &cm_bits,
        shared_ptr<digest_variable<FieldT>> sn_bits,
        const string &annotation_prefix
) :
        gadget<FieldT>(pb, annotation_prefix),
        addr_gadget(pb, ZERO, a_sk_bits, a_pk_bits,
                    FMT(annotation_prefix, " addr_gadget")),
        commitment_gadget(pb, ZERO, a_pk_bits->bits, rho_bits, r_bits, v_bits,
                          cm_bits, FMT(annotation_prefix, " cm_gadget")),
        mt_path_gadget(pb, tree_height, address_bits, cm_bits, rt_bits, path,
                       ONE, FMT(annotation_prefix, " mt_path_gadget")),
        sn_gadget(pb, ZERO, a_sk_bits, rho_bits, sn_bits,
                  FMT(annotation_prefix, " sn_gadget")) {
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void
k0::input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>::generate_r1cs_constraints() {
    addr_gadget.generate_r1cs_constraints();
    commitment_gadget.generate_r1cs_constraints();
    mt_path_gadget.generate_r1cs_constraints();
    sn_gadget.generate_r1cs_constraints();
}

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
void
k0::input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>::generate_r1cs_witness() {
    addr_gadget.generate_r1cs_witness();
    commitment_gadget.generate_r1cs_witness();
    mt_path_gadget.generate_r1cs_witness();
    sn_gadget.generate_r1cs_witness();
}
