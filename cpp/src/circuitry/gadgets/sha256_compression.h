#ifndef ZKTRADE_GADGETS_SHA256_COMPRESSION_H
#define ZKTRADE_GADGETS_SHA256_COMPRESSION_H

using namespace libsnark;

namespace zktrade {

    template<typename FieldT>
    class sha256_compression_gadget : public gadget<FieldT> {
    private:
        std::unique_ptr<sha256_compression_function_gadget<FieldT> > f;
    public:
        sha256_compression_gadget(
                protoboard<FieldT> &pb,
                pb_variable_array<FieldT> &input_1,
                pb_variable_array<FieldT> &input_2,
                digest_variable<FieldT> &result,
                const std::string &annotation_prefix
        );

        sha256_compression_gadget(
                protoboard<FieldT> &pb,
                pb_variable_array<FieldT> &input_1,
                pb_variable_array<FieldT> &input_2,
                pb_variable_array<FieldT> &input_3,
                digest_variable<FieldT> &result,
                const std::string &annotation_prefix
        );

        sha256_compression_gadget(
                protoboard<FieldT> &pb,
                std::vector<pb_variable_array<FieldT>> inputs,
                digest_variable<FieldT> &result,
                const std::string &annotation_prefix
        );

        void generate_r1cs_constraints();

        void generate_r1cs_witness();
    };

};

#include "sha256_compression.tcc"

#endif //ZKTRADE_GADGETS_SHA256_COMPRESSION_H
