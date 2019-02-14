#ifndef ZKTRADE_GADGETS_SHA256_COMPRESSION_TCC
#define ZKTRADE_GADGETS_SHA256_COMPRESSION_TCC

namespace zktrade {

    template<typename FieldT>
    sha256_compression_gadget<FieldT>::sha256_compression_gadget(
            protoboard<FieldT> &pb,
            pb_variable_array<FieldT> &input_1,
            pb_variable_array<FieldT> &input_2,
            digest_variable<FieldT> &result,
            const std::string &annotation_prefix) :
    gadget<FieldT>(pb, annotation_prefix) {
        if (input_1.size() + input_2.size() != 512) {
            throw std::invalid_argument(
                    "Lengths of inputs do not sum up to 512");
        }
        pb_variable_array<FieldT> block;
        block.insert(block.end(), input_1.begin(), input_1.end());
        block.insert(block.end(), input_2.begin(), input_2.end());
        f.reset(
                new sha256_compression_function_gadget<FieldT>(
                        pb,
                        SHA256_default_IV<FieldT>(pb),
                        block,
                        result,
                        FMT(this->annotation_prefix, " hasher")));
    }

    template<typename FieldT>
    sha256_compression_gadget<FieldT>::sha256_compression_gadget(
            protoboard<FieldT> &pb,
            pb_variable_array<FieldT> &input_1,
            pb_variable_array<FieldT> &input_2,
            pb_variable_array<FieldT> &input_3,
            digest_variable<FieldT> &result,
            const std::string &annotation_prefix) :

    gadget<FieldT>(pb, annotation_prefix) {
        if (input_1.size() + input_2.size() + input_3.size() != 512) {
            throw std::invalid_argument(
                    "Lengths of inputs do not sum up to 512");
        }
        pb_variable_array<FieldT>
        block;
        block.insert(block.end(), input_1.begin(), input_1.end());
        block.insert(block.end(), input_2.begin(), input_2.end());
        block.insert(block.end(), input_3.begin(), input_3.end());
        f.reset(
                new sha256_compression_function_gadget<FieldT>(
                        pb,
                        SHA256_default_IV<FieldT>(pb),
                        block,
                        result,
                        FMT(this->annotation_prefix, " hasher")));
    }
    template<typename FieldT>
    sha256_compression_gadget<FieldT>::sha256_compression_gadget(
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
    zktrade::sha256_compression_gadget<FieldT>::generate_r1cs_constraints() {
        f->generate_r1cs_constraints();
    }

    template<typename FieldT>
    void zktrade::sha256_compression_gadget<FieldT>::generate_r1cs_witness() {
        f->generate_r1cs_witness();
    }

}

#endif //ZKTRADE_GADGETS_SHA256_COMPRESSION_TCC
