#include <gtest/gtest.h>
#include <libff/common/utils.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "util.h"
#include "serialization.hpp"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace k0;

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
    cout << "second_half_reversed " << bits_to_hex(second_half_reversed)
         << endl;
    ASSERT_NE(bits_1, second_half);
    ASSERT_EQ(bits_1, second_half_reversed);
}

TEST(BitsAndNumbers, LessThan256Bits) {
    vector<string> input{
            "0x0000000000000000000000000000000000000001",
            "0xffffffffffffffffffffffffffffffffffffffff",
            "0xdf1a27Fc2b2EA68525E3dcc5780CbcbE73e6778A",
            "0x61201b8cBA5CAB5c99935146Cd9c6B99050BC47f",
            "0x43658D5cce59e1A63Ad40630c89423Aa23c4E134",
            "0x7a2dAf3028b903c73eAeF361E20aEBbe4190A1c7",
            "0x4C4eDd3Cc65e6bcA86e01c406252A34b7574b835",
            "0x6Ad5182Eb11A9792B1d74833F64f0FEeB7CA72Be",
            "0xCfF76aF5bFC34dbDAa810ed9a2E9edAb093E3472",
            "0xEeDA3e1669579C04A05E01c50aFA77EDB1c061AA",
            "0xE94Ae3289bE44A946d55e77827fF19F44450cA5b",
            "0x105C92C07f47709aa9926c697DED50e56E3E6dF2"
    };
    vector<string> expected_output{
            "1",
            "1461501637330902918203684832716283019655932542975",
            "1273688244456584839735093243794471170958885812106",
            "554488128535234514404730477514677120558175077503",
            "384767071322830634246376080506636963586398019892",
            "697515668612163741629280368201275722368080191943",
            "435642029203907532051349391777938782044493756469",
            "609905187017876682145402197593137347723770098366",
            "1187278691150957129503052088492597600117512352882",
            "1363606774497317157814832081707217741900230451626",
            "1331864893054062757360647830021747940020301384283",
            "93408304788589627958079886778341295953834569202"
    };
    vector<string> output(input.size());
    transform(input.begin(), input.end(), output.begin(), hex_to_dec_string);
    ASSERT_EQ(output, expected_output);
}
