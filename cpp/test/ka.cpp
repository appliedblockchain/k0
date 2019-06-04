#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/gadgets/sha256_compression.h"
#include "scheme/prfs.h"
#include "scheme/ka.hpp"
#include "serialization.hpp"
#include "util.h"

using namespace std;
using namespace libff;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_compression_gadget<FieldT> HashT;

TEST(KA, FormatPrivate) {
    auto a_sk = random_bits(256);
    auto prfed = prf_addr_sk_enc<HashT>(a_sk);
    auto sk_enc = ka_format_private(prfed);

    // TODO maybe change (bit order). See ZCash extended paper page 58
    ASSERT_FALSE(sk_enc[7]);
    ASSERT_FALSE(sk_enc[6]);
    ASSERT_FALSE(sk_enc[5]);
    ASSERT_TRUE(sk_enc[31 * 8 + 1]);
    ASSERT_FALSE(sk_enc[31 * 8 + 0]);
    cout << "LENGTH " << sk_enc.size() << endl;
}

TEST(KA, DerivePublic) {
    auto a_sk = random_bits(256);
    auto prfed = prf_addr_sk_enc<HashT>(a_sk);
    auto sk_enc = ka_format_private(prfed);
    auto pk_enc = ka_derive_public(sk_enc);
    cout << "PK " << bits2hex(pk_enc) << endl;
}

TEST(KA, Agree) {
    auto alice_a_sk = random_bits(256);
    auto alice_sk_enc = ka_format_private(prf_addr_sk_enc<HashT>(alice_a_sk));
    auto alice_pk_enc = ka_derive_public(alice_sk_enc);

    auto bob_a_sk = random_bits(256);
    auto bob_sk_enc = ka_format_private(prf_addr_sk_enc<HashT>(bob_a_sk));
    auto bob_pk_enc = ka_derive_public(bob_sk_enc);

    auto alice_secret = ka_agree(alice_sk_enc, bob_pk_enc);
    auto bob_secret = ka_agree(bob_sk_enc, alice_pk_enc);

    ASSERT_EQ(alice_secret, bob_secret);
}
