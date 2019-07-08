#ifndef K0_GADGETS_SHA256_COMPRESSION_TCC
#define K0_GADGETS_SHA256_COMPRESSION_TCC

template<typename FieldT>
k0::sha256_compression_gadget<FieldT>::sha256_compression_gadget(
        protoboard<FieldT> &pb,
        std::vector<pb_variable_array<FieldT>> inputs,
        digest_variable<FieldT> &result,
        const std::string &annotation_prefix) :

        gadget<FieldT>(pb, annotation_prefix) {
    block_variable<FieldT> block{pb, inputs, "hasher input"};
    f.reset(
            new sha256_compression_function_gadget<FieldT>(
                    pb,
                    SHA256_default_IV<FieldT>(pb),
                    block.bits,
                    result,
                    FMT(this->annotation_prefix, " hasher")));
}


template<typename FieldT>
void
k0::sha256_compression_gadget<FieldT>::generate_r1cs_constraints() {
    f->generate_r1cs_constraints();
}

template<typename FieldT>
void k0::sha256_compression_gadget<FieldT>::generate_r1cs_witness() {
    f->generate_r1cs_witness();
}

template<typename FieldT>
libff::bit_vector k0::sha256_compression_gadget<FieldT>::get_hash(const libff::bit_vector &input) {
    protoboard<FieldT> pb;

    pb_variable_array<FieldT> input_va;
    input_va.allocate(pb, 512, "input");
    digest_variable<FieldT> output(pb, SHA256_digest_size, "output");
    sha256_compression_gadget f(pb, {input_va}, output, "f");
    input_va.fill_with_bits(pb, input);
    f.generate_r1cs_witness();

    return output.get_digest();
}

#endif //K0_GADGETS_SHA256_COMPRESSION_TCC
