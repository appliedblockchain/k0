#ifndef K0_GADGETS_SHA256_512_BITS_TCC
#define K0_GADGETS_SHA256_512_BITS_TCC

// Adapted from the commitment gadget in ZCash


template<typename FieldT>
pb_variable_array<FieldT>
from_bits(std::vector<bool> bits, pb_variable<FieldT> &ZERO) {
    pb_variable_array<FieldT> acc;

    for (bool bit : bits) {
        acc.emplace_back(bit ? ONE : ZERO);
    }

    return acc;
}

template<typename FieldT>
k0::sha256_512_bits_gadget<FieldT>::sha256_512_bits_gadget(
        protoboard<FieldT> &pb, pb_variable<FieldT> &ZERO,
        block_variable<FieldT> &block1, digest_variable<FieldT> &result,
        const std::string &annotation_prefix) :
        gadget<FieldT>(pb, annotation_prefix) {

    pb_variable_array<FieldT> leading_byte = from_bits(
            {1, 0, 1, 1, 0, 0, 0, 0}, ZERO);

    intermediate_hash.reset(new digest_variable<FieldT>(
            pb, 256, FMT(annotation_prefix, " intermediate_hash")));

    // final padding
    pb_variable_array<FieldT> length_padding = from_bits(
            {
                    // padding
                    1, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,

                    // length of message (512 bits)
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
            }, ZERO);


    block2.reset(new block_variable<FieldT>(
            pb, {length_padding}, FMT(annotation_prefix, " block 2")));

    pb_linear_combination_array<FieldT> IV = SHA256_default_IV(pb);

    hasher1.reset(new sha256_compression_function_gadget<FieldT>(
            pb, IV, block1.bits, *intermediate_hash,
            FMT(annotation_prefix, "hasher1")));

    pb_linear_combination_array<FieldT> IV2(intermediate_hash->bits);

    hasher2.reset(new sha256_compression_function_gadget<FieldT>(
            pb, IV2, block2->bits, result, FMT(annotation_prefix, "hasher2")));
}


template<typename FieldT>
void k0::sha256_512_bits_gadget<FieldT>::generate_r1cs_constraints() {
    hasher1->generate_r1cs_constraints();
    hasher2->generate_r1cs_constraints();
}

template<typename FieldT>
void k0::sha256_512_bits_gadget<FieldT>::generate_r1cs_witness() {
    hasher1->generate_r1cs_witness();
    hasher2->generate_r1cs_witness();
}

#endif //K0_GADGETS_SHA256_512_BITS_TCC
