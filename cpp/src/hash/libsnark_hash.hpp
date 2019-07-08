#ifndef K0_LIBSNARK_HASH_HPP
#define K0_LIBSNARK_HASH_HPP

#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>

using namespace libff;
using namespace libsnark;

namespace k0 {

    template<typename FieldT>
    bit_vector libsnark_hash(bit_vector input) {
        protoboard<FieldT> pb;

        block_variable <FieldT> input_variable(pb, 512, "input");
        digest_variable <FieldT> output_variable(pb, 256, "output");
        sha256_compression_function_gadget <FieldT> f(pb, SHA256_default_IV<FieldT>(pb), input_variable.bits,
                                                      output_variable, "f");

        input_variable.generate_r1cs_witness(input);
        f.generate_r1cs_witness();

        return output_variable.get_digest();
    }

}
#endif //K0_LIBSNARK_HASH_HPP
