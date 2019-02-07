#include "gtest/gtest.h"
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>

int main(int argc, char **argv) {
    libsnark::default_r1cs_ppzksnark_pp::init_public_params();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
