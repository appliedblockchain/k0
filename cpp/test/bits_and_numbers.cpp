#include <gtest/gtest.h>
#include <libff/common/utils.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "util.h"

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
