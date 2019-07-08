#ifndef K0_SHA256_COMPRESSION_HPP
#define K0_SHA256_COMPRESSION_HPP

#include <libff/common/utils.hpp>

namespace k0 {
    // void sha256_compress(unsigned char* input, unsigned char* output);
    libff::bit_vector sha256_compress(libff::bit_vector input);
}

#endif //K0_SHA256_COMPRESSION_HPP
