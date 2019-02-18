#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "util.h"

using namespace std;
using namespace libff;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

struct byte_to_bits_example {
    unsigned char b;
    bit_vector bv;
};

TEST(Util, byte_to_bits_to_byte) {
    vector<byte_to_bits_example> examples{
            {1,   {0, 0, 0, 0, 0, 0, 0, 1}},
            {2,   {0, 0, 0, 0, 0, 0, 1, 0}},
            {4,   {0, 0, 0, 0, 0, 1, 0, 0}},
            {8,   {0, 0, 0, 0, 1, 0, 0, 0}},
            {16,  {0, 0, 0, 1, 0, 0, 0, 0}},
            {32,  {0, 0, 1, 0, 0, 0, 0, 0}},
            {64,  {0, 1, 0, 0, 0, 0, 0, 0}},
            {128, {1, 0, 0, 0, 0, 0, 0, 0}},
            {255, {1, 1, 1, 1, 1, 1, 1, 1}}
    };
    for (auto example : examples) {
        bit_vector bv = byte_to_bits(example.b);
        ASSERT_EQ(bv, example.bv);

        unsigned char b = bits_to_byte(example.bv);
        ASSERT_EQ(b, example.b);
    }
    for (int i = 0; i < 255; i++) {
        ASSERT_EQ(i, bits_to_byte(byte_to_bits(i)));
    }
}

struct concat_example {
    bit_vector a;
    bit_vector b;
    bit_vector result;
};

TEST(Util, concat) {
    vector<concat_example> examples{
            {{},                    {},              {}},
            {{0},                   {1},             {0, 1}},
            {{1, 0, 0, 0, 1, 1, 1}, {0, 1, 0, 1, 1}, {1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1}}
    };
    for (auto example : examples) {
        ASSERT_EQ(concat(example.a, example.b), example.result);
    }
}

TEST(Util, bytes_to_bits) {

    struct bytes_to_bits_example {
        vector<unsigned char> bytes;
        libff::bit_vector bits;
    };
    vector<bytes_to_bits_example> examples{
            {{},        {}},
            {{0},       {0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 4, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    };
    for (auto example : examples) {
        ASSERT_EQ(bytes_to_bits(example.bytes), example.bits);
    }
}

TEST(Util, truncate) {
    struct truncate_example {
        bit_vector in;
        bit_vector out;
    };
    vector<truncate_example> examples{
            {{1, 0, 1}, {1, 0}},
            {{1, 0, 1}, {1}}
    };
    for (auto example: examples) {
        ASSERT_EQ(truncate(example.in, example.out.size()), example.out);
    }
}

TEST(Util, field_element_to_64_bits) {
    bit_vector result = field_element_to_64_bits(FieldT("1000000000000000000"));
    cout << bits_to_hex(result) << endl;

    uint64_t uint64_val = 1000000000000000000;
    cout << uint64_to_string(uint64_val) << endl;
}

TEST(Util, IntBitsConversion) {
    for (auto i = 0; i < 100; i++) {
        uint64_t val = random_uint64();
        cout << dec << val << endl;
        cout << hex << val << endl;
        bit_vector bv{uint64_to_bits(val)};
        cout << bits_to_hex(bv) << endl;
        ASSERT_EQ(bits_to_uint64(bv), val);
    }
}