#include <gtest/gtest.h>
#include <libff/common/utils.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "util.h"
#include "serialization.hpp"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

TEST(BitsAndNumbers, PackingBitOrder) {
    uint64_t v = 5;

    // 0x0000...0101
    auto v_bv_libff = int_list_to_bits({v}, 64);

    protoboard<FieldT> pb;
    pb_variable<FieldT> v_variable;
    v_variable.allocate(pb, "v_variable");
    pb_variable_array<FieldT> v_bits;
    v_bits.allocate(pb, 64, "v_bits");
    packing_gadget<FieldT> packer(pb, v_bits, v_variable, "packer");
    packer.generate_r1cs_constraints(false);
    pb.val(v_variable) = v;
    packer.generate_r1cs_witness_from_packed();
    // 0x1010...000
    auto v_bv_unpacked = v_bits.get_bits(pb);

    ASSERT_NE(v_bv_unpacked, v_bv_libff);

    reverse(v_bv_unpacked.begin(), v_bv_unpacked.end());
    ASSERT_EQ(v_bv_unpacked, v_bv_libff);
}

TEST(BitsAndNumbers, MultiPacking) {
    bit_vector input = random_bits(256);
    bit_vector first_half{input.begin(), input.begin() + 128};
    bit_vector first_half_reversed{first_half.rbegin(), first_half.rend()};
    bit_vector second_half{input.begin() + 128, input.end()};
    bit_vector second_half_reversed{second_half.rbegin(), second_half.rend()};

    cout << bits_to_hex(input) << endl;
    cout << bits_to_hex(first_half) << endl;
    cout << bits_to_hex(second_half) << endl;

    protoboard<FieldT> pb;
    pb_variable_array<FieldT> bits;
    bits.allocate(pb, 256, "bits");
    pb_variable_array<FieldT> packed;
    packed.allocate(pb, 2, "packed");
    multipacking_gadget<FieldT> packer(pb, bits, packed, 128, "packer");
    packer.generate_r1cs_constraints(false);
    bits.fill_with_bits(pb, input);
    packer.generate_r1cs_witness_from_bits();

    auto val_0 = pb.val(packed[0]);
    stringstream hex_0;
    hex_0 << std::setfill('0') << std::setw(1) << std::hex << val_0;
    auto bits_0 = hex2bits("0x" + hex_0.str());
    cout << "bits_0              " << bits_to_hex(bits_0) << endl;
    cout << "first_half          " << bits_to_hex(first_half) << endl;
    cout << "first_half_reversed " << bits_to_hex(first_half_reversed) << endl;
    ASSERT_NE(bits_0, first_half);
    ASSERT_EQ(bits_0, first_half_reversed);

    stringstream hex_1;
    hex_1 << std::setfill('0') << std::setw(1) << std::hex << pb.val(packed[1]);
    auto bits_1 = hex2bits("0x" + hex_1.str());
    cout << "bits_1               " << bits_to_hex(bits_1) << endl;
    cout << "second_half          " << bits_to_hex(second_half) << endl;
    cout << "second_half_reversed " << bits_to_hex(second_half_reversed) << endl;
    ASSERT_NE(bits_1, second_half);
    ASSERT_EQ(bits_1, second_half_reversed);
}
