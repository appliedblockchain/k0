#ifndef ZKTRADE_UTIL_H
#define ZKTRADE_UTIL_H

#include <libff/common/utils.hpp>

namespace zktrade {
    libff::bit_vector byte_to_bits(unsigned char b);
    unsigned char bits_to_byte(libff::bit_vector bv);
    libff::bit_vector concat(libff::bit_vector &a, libff::bit_vector &b);
    libff::bit_vector bytes_to_bits(std::vector<unsigned char> &bytes);
    std::vector<unsigned char> bits_to_bytes(libff::bit_vector bits);
    std::string bytes_to_hex(std::vector<unsigned char> bytes);
    libff::bit_vector random_bits(size_t len);
}

#endif //ZKTRADE_UTIL_H
