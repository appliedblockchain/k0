#ifndef ZKTRADE_SHA256_HPP
#define ZKTRADE_SHA256_HPP

#include <libff/common/utils.hpp>

namespace zktrade {
    void sha256(unsigned char* input, unsigned char* output);
    libff::bit_vector sha256(libff::bit_vector input);
}

#endif //ZKTRADE_SHA256_HPP
