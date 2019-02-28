#ifndef ZKTRADE_DUMMYHASH_GADGET_TCC
#define ZKTRADE_DUMMYHASH_GADGET_TCC

template<typename FieldT>
zktrade::dummyhash_compression_function_gadget<FieldT>::dummyhash_compression_function_gadget(
        protoboard<FieldT>
        &pb,
        const pb_linear_combination_array<FieldT> &prev_output,
        const pb_variable_array<FieldT> &new_block,
        const digest_variable<FieldT> &output,
        const std::string &annotation_prefix
) :
        gadget<FieldT>(pb, annotation_prefix),
        A(new_block.begin(), new_block.begin() + 256),
        B(new_block.begin() + 256, new_block.end()),
        prev_output(prev_output),
        new_block(new_block),
        output(output) {
    C.allocate(pb, 256, FMT(annotation_prefix, " C"));
    for (size_t i = 0; i < 256; i++) {
        pb.val(C[i]) = i % 2 ? FieldT::one() : FieldT::zero();
    }
    tmp.allocate(pb, 256, FMT(annotation_prefix, " tmp"));
}


template<typename FieldT>
void
zktrade::dummyhash_compression_function_gadget<FieldT>::generate_r1cs_constraints() {
    // Taken from libsnark XOR3 gadget
    // tmp = A + B - 2AB i.e. tmp = A xor B
    // out = tmp + C - 2tmp C i.e. out = tmp xor C
    for (size_t i = 0; i < 250; i++) {
        this->pb.add_r1cs_constraint(
                r1cs_constraint<FieldT>( 2 * A[i], B[i], A[i] + B[i] - tmp[i]),
                FMT(this->annotation_prefix, " tmp"));
        this->pb.add_r1cs_constraint(
                r1cs_constraint<FieldT>(
                        2 * tmp[i], C[i], tmp[i] + C[i] - output.bits[i]),
                        FMT(this->annotation_prefix, " out"));
    }

}

template<typename FieldT>
void
zktrade::dummyhash_compression_function_gadget<FieldT>::generate_r1cs_witness() {

#ifdef DEBUG
    printf("Input:\n");
    for (size_t i = 0; i < 256; ++i) {
        printf("%lx ", this->pb.val(this->new_block[i]).as_ulong());
    }
    printf("\n");
    for (size_t i = 0; i < 256; ++i) {
        printf("%lx ", this->pb.val(this->new_block[256 + i]).as_ulong());
    }
    printf("\n");
#endif

    for (size_t i = 0; i < 256; i++) {
        this->pb.val(tmp[i]) = this->pb.lc_val(A[i]) + this->pb.lc_val(B[i]) -
                               FieldT(2) * this->pb.lc_val(A[i]) *
                               this->pb.lc_val(B[i]);
        this->pb.lc_val(output.bits[i]) =
                this->pb.val(tmp[i]) + this->pb.lc_val(C[i]) -
                FieldT(2) * this->pb.val(tmp[i]) * this->pb.lc_val(C[i]);
    }

#ifdef DEBUG
    printf("Output:\n");
    for (size_t i = 0; i < 256; ++i) {
        printf("%lx ", this->pb.val(output.bits[i]).as_ulong());
    }
    printf("\n");
#endif
}


template<typename FieldT>
zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::dummyhash_two_to_one_hash_gadget(
        protoboard<FieldT> &pb,
        const digest_variable<FieldT> &left,
        const digest_variable<FieldT> &right,
        const digest_variable<FieldT> &output,
        const std::string &annotation_prefix) :
        gadget<FieldT>(pb, annotation_prefix) {
    /* concatenate block = left || right */
    pb_variable_array<FieldT> block;
    block.insert(block.end(), left.bits.begin(), left.bits.end());
    block.insert(block.end(), right.bits.begin(), right.bits.end());

    /* compute the hash itself */
    f.reset(new dummyhash_compression_function_gadget<FieldT>(pb,
                                                              SHA256_default_IV<FieldT>(
                                                                      pb),
                                                              block, output,
                                                              FMT(this->annotation_prefix,
                                                                  " f")));
}

template<typename FieldT>
zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::dummyhash_two_to_one_hash_gadget(
        protoboard<FieldT> &pb,
        const size_t block_length,
        const block_variable<FieldT> &input_block,
        const digest_variable<FieldT> &output,
        const std::string &annotation_prefix) :
        gadget<FieldT>(pb, annotation_prefix) {
    assert(block_length == SHA256_block_size);
    assert(input_block.bits.size() == block_length);
    f.reset(new zktrade::dummyhash_compression_function_gadget<FieldT>(pb,
                                                                       SHA256_default_IV<FieldT>(
                                                                               pb),
                                                                       input_block.bits,
                                                                       output,
                                                                       FMT(this->annotation_prefix,
                                                                           " f")));
}

template<typename FieldT>
void
zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::generate_r1cs_constraints(
        const bool ensure_output_bitness) {
    libff::UNUSED(ensure_output_bitness);
    f->generate_r1cs_constraints();
}

template<typename FieldT>
void
zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::generate_r1cs_witness() {
    f->generate_r1cs_witness();
}

template<typename FieldT>
size_t zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::get_block_len() {
    return SHA256_block_size;
}

template<typename FieldT>
size_t zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::get_digest_len() {
    return SHA256_digest_size;
}

template<typename FieldT>
libff::bit_vector zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::get_hash(
        const libff::bit_vector &input) {
    protoboard<FieldT> pb;

    block_variable<FieldT> input_variable(pb, SHA256_block_size, "input");
    digest_variable<FieldT> output_variable(pb, SHA256_digest_size, "output");
    dummyhash_two_to_one_hash_gadget <FieldT> f(pb, SHA256_block_size,
                                                input_variable, output_variable,
                                                "f");

    input_variable.generate_r1cs_witness(input);
    f.generate_r1cs_witness();

    return output_variable.get_digest();
}

template<typename FieldT>
size_t zktrade::dummyhash_two_to_one_hash_gadget<FieldT>::expected_constraints(
        const bool ensure_output_bitness) {
    libff::UNUSED(ensure_output_bitness);
    return 27280; /* hardcoded for now */
}

template<typename FieldT>
zktrade::dummyhash_compression_gadget<FieldT>::dummyhash_compression_gadget(
        protoboard<FieldT> &pb,
        std::vector<pb_variable_array<FieldT>> inputs,
        digest_variable<FieldT> &result,
        const std::string &annotation_prefix) :

        gadget<FieldT>(pb, annotation_prefix) {
    block_variable<FieldT> block{pb, inputs, "hasher input"};
    f.reset(
            new dummyhash_compression_function_gadget<FieldT>(
                    pb,
                    SHA256_default_IV<FieldT>(pb),
                    block.bits,
                    result,
                    FMT(this->annotation_prefix, " hasher")));
}


template<typename FieldT>
void
zktrade::dummyhash_compression_gadget<FieldT>::generate_r1cs_constraints() {
    f->generate_r1cs_constraints();
}

template<typename FieldT>
void zktrade::dummyhash_compression_gadget<FieldT>::generate_r1cs_witness() {
    f->generate_r1cs_witness();
}

template<typename FieldT>
libff::bit_vector zktrade::dummyhash_compression_gadget<FieldT>::get_hash(
        const libff::bit_vector &input) {
    protoboard<FieldT> pb;

    pb_variable_array<FieldT> input_va;
    input_va.allocate(pb, 512, "input");
    digest_variable<FieldT> output(pb, SHA256_digest_size, "output");
    dummyhash_compression_gadget f(pb, {input_va}, output, "f");
    input_va.fill_with_bits(pb, input);
    f.generate_r1cs_witness();

    return output.get_digest();
}

#endif // ZKTRADE_DUMMYHASH_GADGET_TCC
