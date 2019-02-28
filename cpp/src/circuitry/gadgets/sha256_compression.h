#ifndef ZKTRADE_GADGETS_SHA256_COMPRESSION_H
#define ZKTRADE_GADGETS_SHA256_COMPRESSION_H

#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>

using namespace libsnark;

namespace zktrade {

    template<typename FieldT>
    class sha256_compression_gadget : public gadget<FieldT> {
    private:
        std::unique_ptr<sha256_compression_function_gadget<FieldT> > f;
    public:
        sha256_compression_gadget(
                protoboard<FieldT> &pb,
                std::vector<pb_variable_array<FieldT>> inputs,
                digest_variable<FieldT> &result,
                const std::string &annotation_prefix
        );
        void generate_r1cs_constraints();
        void generate_r1cs_witness();

        static libff::bit_vector get_hash(const libff::bit_vector &input);
    };

};

#include "sha256_compression.tcc"

#endif //ZKTRADE_GADGETS_SHA256_COMPRESSION_H
