#ifndef K0_DUMMYHASH_GADGET_TCC
#define K0_DUMMYHASH_GADGET_TCC

using namespace std;

template<typename FieldT>
k0::dummyhash_knapsack_wrapper_gadget<FieldT>::dummyhash_knapsack_wrapper_gadget(
        protoboard<FieldT> &pb,
        const block_variable<FieldT> &block,
        const digest_variable<FieldT> &output,
        const std::string &annotation_prefix)
        : gadget<FieldT>(pb, annotation_prefix),
                pb(&pb),
                block(&block),
                output(&output),
          knapsack_output(pb, 254, FMT(annotation_prefix, "knapsack_output")),
          knapsack_crh(pb, 512, block, knapsack_output,
                       FMT(annotation_prefix, " knapsack_crh")) {
}

template<typename FieldT>
void
k0::dummyhash_knapsack_wrapper_gadget<FieldT>::generate_r1cs_constraints() {
    knapsack_crh.generate_r1cs_constraints();
    for (size_t i = 0; i < 254; i++) {
        pb->add_r1cs_constraint(
                r1cs_constraint<FieldT>(output->bits[i], FieldT::one(), knapsack_output.bits[i]), "knapsack");
    }
    pb->add_r1cs_constraint(r1cs_constraint<FieldT>(output->bits[255], FieldT::one(), FieldT::zero()), "knapsack");
}

template<typename FieldT>
void
k0::dummyhash_knapsack_wrapper_gadget<FieldT>::generate_r1cs_witness() {
    knapsack_crh.generate_r1cs_witness();
    for (size_t i = 0; i < 254; i++) {
        pb->val(output->bits[i]) = pb->val(knapsack_output.bits[i]);
    }
    pb->val(output->bits[255]) = FieldT::zero();
}


template<typename FieldT>
k0::dummyhash_two_to_one_hash_gadget<FieldT>::dummyhash_two_to_one_hash_gadget(
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
    f.reset(new dummyhash_knapsack_wrapper_gadget<FieldT>(
            pb, block, output, FMT(this->annotation_prefix, " f")));
}

template<typename FieldT>
k0::dummyhash_two_to_one_hash_gadget<FieldT>::dummyhash_two_to_one_hash_gadget(
        protoboard<FieldT> &pb,
        const size_t block_length,
        const block_variable<FieldT> &input_block,
        const digest_variable<FieldT> &output,
        const std::string &annotation_prefix) :
        gadget<FieldT>(pb, annotation_prefix) {
    assert(block_length == 512);
    assert(input_block.bits.size() == block_length);
    f.reset(new dummyhash_knapsack_wrapper_gadget<FieldT>(
            pb, input_block, output, FMT(this->annotation_prefix, " f")));
}

template<typename FieldT>
void
k0::dummyhash_two_to_one_hash_gadget<FieldT>::generate_r1cs_constraints(
        const bool ensure_output_bitness) {
    libff::UNUSED(ensure_output_bitness);
    f->generate_r1cs_constraints();
}

template<typename FieldT>
void
k0::dummyhash_two_to_one_hash_gadget<FieldT>::generate_r1cs_witness() {
    f->generate_r1cs_witness();
}

template<typename FieldT>
size_t k0::dummyhash_two_to_one_hash_gadget<FieldT>::get_block_len() {
    return 512;
}

template<typename FieldT>
size_t k0::dummyhash_two_to_one_hash_gadget<FieldT>::get_digest_len() {
    return 256;
}

template<typename FieldT>
libff::bit_vector k0::dummyhash_two_to_one_hash_gadget<FieldT>::get_hash(
        const libff::bit_vector &input) {
    protoboard<FieldT> pb;

    block_variable<FieldT> input_variable(pb, 512, "input");
    digest_variable<FieldT> output_variable(pb, 256, "output");
    dummyhash_two_to_one_hash_gadget <FieldT> f(pb, 512,
                                                input_variable, output_variable,
                                                "f");

    input_variable.generate_r1cs_witness(input);
    f.generate_r1cs_witness();

    return output_variable.get_digest();
}

template<typename FieldT>
size_t k0::dummyhash_two_to_one_hash_gadget<FieldT>::expected_constraints(
        const bool ensure_output_bitness) {
    libff::UNUSED(ensure_output_bitness);
    return 27280; /* hardcoded for now */
}

template<typename FieldT>
k0::dummyhash_compression_gadget<FieldT>::dummyhash_compression_gadget(
        protoboard<FieldT> &pb,
        std::vector<pb_variable_array<FieldT>> inputs,
        digest_variable<FieldT> &result,
        const std::string &annotation_prefix) :

        gadget<FieldT>(pb, annotation_prefix) {
    block_variable<FieldT> block{pb, inputs, "hasher input"};
    f.reset(
            new dummyhash_knapsack_wrapper_gadget<FieldT>(
                    pb,
                    block,
                    result,
                    FMT(this->annotation_prefix, " hasher")));
}


template<typename FieldT>
void
k0::dummyhash_compression_gadget<FieldT>::generate_r1cs_constraints() {
    f->generate_r1cs_constraints();
}

template<typename FieldT>
void k0::dummyhash_compression_gadget<FieldT>::generate_r1cs_witness() {
    f->generate_r1cs_witness();
}

template<typename FieldT>
libff::bit_vector k0::dummyhash_compression_gadget<FieldT>::get_hash(
        const libff::bit_vector &input) {
    protoboard<FieldT> pb;

    pb_variable_array<FieldT> input_va;
    input_va.allocate(pb, 512, "input");
    digest_variable<FieldT> output(pb, 256, "output");
    dummyhash_compression_gadget f(pb, {input_va}, output, "f");
    input_va.fill_with_bits(pb, input);
    f.generate_r1cs_witness();

    return output.get_digest();
}

#endif // K0_DUMMYHASH_GADGET_TCC
