#ifndef ZKTRADE_GADGETS_SHA256_512_BITS_HPP
#define ZKTRADE_GADGETS_SHA256_512_BITS_HPP

using namespace libsnark;

namespace zktrade {

    template<typename FieldT>
    class sha256_512_bits_gadget : public gadget<FieldT> {
    private:
    private:
        std::shared_ptr<block_variable<FieldT>> block2;
        std::shared_ptr<sha256_compression_function_gadget<FieldT>> hasher1;
        std::shared_ptr<digest_variable<FieldT>> intermediate_hash;
        std::shared_ptr<sha256_compression_function_gadget<FieldT>> hasher2;
    public:
        sha256_512_bits_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                block_variable<FieldT> &block1,
                digest_variable<FieldT> &result,
                const std::string &annotation_prefix
        );

        void generate_r1cs_constraints();

        void generate_r1cs_witness();
    };

};

#include "sha256_512_bits.tcc"

#endif //ZKTRADE_GADGETS_SHA256_HPP