#include "util.h"
#include <iomanip>

libff::bit_vector zktrade::byte_to_bits(unsigned char b) {
    libff::bit_vector bv(8);
    for (size_t i = 0; i < 8; ++i) {
        bv[i] = b & (1ul << (8 - 1 - i));
    }
    return bv;
}

unsigned char zktrade::bits_to_byte(libff::bit_vector bv) {
    if (bv.size() != 8) {
        throw std::invalid_argument("bit vector is not of length 8");
    }
    unsigned char b = 0;
    for (size_t i = 0; i < 8; i++) {
        // get the current bit
        unsigned char j = bv[i] ? 1 : 0;
        // OR b with the current bit
        b |= j;
        if (i < 7) {
            // shift b 1 bit to the left
            b = b << 1;
        }
    }
    return b;
}

libff::bit_vector zktrade::concat(libff::bit_vector &a, libff::bit_vector &b) {
    libff::bit_vector result;
    for (auto bit : a) {
        result.push_back(bit);
    }
    for (auto bit : b) {
        result.push_back(bit);
    }
    return result;
}

libff::bit_vector zktrade::concat(libff::bit_vector &a, libff::bit_vector &b, libff::bit_vector &c) {
    libff::bit_vector result;
    for (auto bit : a) {
        result.push_back(bit);
    }
    for (auto bit : b) {
        result.push_back(bit);
    }
    for (auto bit : c) {
        result.push_back(bit);
    }
    return result;
}

libff::bit_vector zktrade::bytes_to_bits(std::vector<unsigned char> &bytes) {
    libff::bit_vector result;
    for (auto b : bytes) {
        libff::bit_vector bv = byte_to_bits(b);
        for (auto bit : bv) {
            result.push_back(bit);
        }
    }
    return result;
}

std::vector<unsigned char> zktrade::bits_to_bytes(libff::bit_vector bits) {
    if (bits.size() % 8 != 0) {
        throw std::invalid_argument(
                "size of bit vector is not a multiple of 8");
    }
    std::vector<unsigned char> result;
    for (int i = 0; i < bits.size(); i = i + 8) {
        libff::bit_vector byte_bv = libff::bit_vector(
                bits.begin() + i, bits.begin() + i + 8);
        result.push_back(bits_to_byte(byte_bv));
    }
    return result;
}

std::string zktrade::bytes_to_hex(std::vector<unsigned char> bytes) {
    std::stringstream stream;
    stream << "0x";
    for (auto b : bytes) {
        stream << std::hex << std::setfill('0') << std::setw(2) << (int) b;
    }
    return stream.str();
}

libff::bit_vector zktrade::random_bits(size_t len = 256) {
    libff::bit_vector v(len);
    generate(v.begin(), v.end(), [&]() { return rand() % 2; });
    return v;
}

bit_vector zktrade::truncate(bit_vector input, size_t len) {
    if (len >= input.size()) {
        throw invalid_argument(
                "Desired output length not smaller than the size of the input bit vector");
    }
    bit_vector shortened(input.begin(), input.begin() + len);
    return shortened;
}

string zktrade::bits_to_string(bit_vector input) {
    stringstream stream;
    for (bool bit : input) {
        stream << (bit ? "1" : "0");
    }
    return stream.str();
}

string zktrade::bits_to_hex(bit_vector input) {
    return bytes_to_hex(bits_to_bytes(input));
}

bit_vector zktrade::zero_bits(size_t len) {
    bit_vector result(len);
    std::fill(result.begin(), result.end(), 0);
    return result;
}