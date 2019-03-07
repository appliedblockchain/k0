#include <gtest/gtest.h>

#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/relations/variable.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include "util.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

TEST(Summation, Prelude) {

    variable<FieldT> x0(0);
    variable<FieldT> x1(1);
    variable<FieldT> x2(2);

    auto lin_term_0 = 5 * x0;
    auto lin_term_1 = 5 * x1;
    auto lin_term_2 = 6 * x2;

    linear_combination<FieldT> lin_comb({lin_term_0, lin_term_1, lin_term_2});
    vector<FieldT> assignment = {FieldT("9"), FieldT("3")};
    ASSERT_EQ(lin_comb.evaluate(assignment), FieldT("68"));
}

TEST(Summation, Test) {
    protoboard<FieldT> pb;

    pb_variable_array<FieldT> a_bits;
    a_bits.allocate(pb, 64, "a_bits");

    pb_variable_array<FieldT> b_bits;
    b_bits.allocate(pb, 64, "b_bits");

    pb_variable_array<FieldT> result_bits;
    result_bits.allocate(pb, 64, "result_bits");

    pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
            1,
            pb_packing_sum<FieldT>(a_bits) + pb_packing_sum<FieldT>(b_bits),
            pb_packing_sum<FieldT>(result_bits)
    ), "sum must equal right_side");

    a_bits.fill_with_bits_of_field_element(pb, FieldT("3"));
    b_bits.fill_with_bits_of_field_element(pb, FieldT("4"));
    result_bits.fill_with_bits_of_field_element(pb, FieldT("8"));

    ASSERT_FALSE(pb.is_satisfied());
    result_bits.fill_with_bits_of_field_element(pb, FieldT("7"));
    ASSERT_TRUE(pb.is_satisfied());
}
