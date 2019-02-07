#ifndef ZKTRADE_SHA256_COMPRESSION_H
#define ZKTRADE_SHA256_COMPRESSION_H

#include <libff/common/utils.hpp>

namespace zktrade {
    void sha256_compress(unsigned char* input, unsigned char* output);
    libff::bit_vector sha256_compress(libff::bit_vector input);
}

#endif //ZKTRADE_SHA256_COMPRESSION_H
