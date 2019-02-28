#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include "scheme/prfs.h"
#include "util.h"
#include "hash/sha256_compression.hpp"
#include "circuitry/gadgets/prfs.hpp"
#include "circuitry/gadgets/sha256_compression.h"

using namespace libff;
using namespace libsnark;
using namespace std;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_compression_gadget<FieldT> HashT;

TEST(prfs, prf_addr) {
    bit_vector a_sk = random_bits(256);
    bit_vector zeros(256);
    std::fill(zeros.begin(), zeros.end(), 0);
    bit_vector expected_result = sha256_compress(concat(a_sk, zeros));
    ASSERT_EQ(prf_addr<HashT>(a_sk), expected_result);
}

TEST(prfs, prf_sn) {
    bit_vector x = random_bits(256);
    bit_vector y{0, 1};
    bit_vector z = random_bits(256);
    bit_vector z_truncated = truncate(z, 254);
    bit_vector right_side = concat(y, z_truncated);
    bit_vector expected_result = sha256_compress(concat(x, right_side));
    ASSERT_EQ(prf_sn<HashT>(x, z), expected_result);

    protoboard<FieldT> pb;
    pb_variable<FieldT> ZERO;
    ZERO.allocate(pb, "ZERO");
    pb_variable_array<FieldT> a_sk;
    a_sk.allocate(pb, 256, "a_sk");
    pb_variable_array<FieldT> rho_truncated;
    rho_truncated.allocate(pb, 254, "rho_truncated");
    auto result =
            make_shared<digest_variable<FieldT>>(pb, 256, "prf_sn_result");
    prf_sn_gadget<FieldT, HashT> prfsng{
            pb, ZERO, a_sk, rho_truncated, result, "prn_sn"};
    ASSERT_TRUE(pb.is_satisfied());
    prfsng.generate_r1cs_constraints();
    ASSERT_FALSE(pb.is_satisfied());
    a_sk.fill_with_bits(pb, x);
    rho_truncated.fill_with_bits(pb, truncate(z, 254));
    ASSERT_FALSE(pb.is_satisfied());
    prfsng.generate_r1cs_witness();
    ASSERT_TRUE(pb.is_satisfied());
    ASSERT_EQ(result->get_digest(), expected_result);
}
