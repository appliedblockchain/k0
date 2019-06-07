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

TEST(KA, Agree) {
    auto alice_a_sk = random_bits(256);
    unsigned char alice_sk_enc[32];
    auto alice_prfed = prf_addr_sk_enc<HashT>(alice_a_sk);
    fill_with_bits(alice_sk_enc, alice_prfed);
    ka_format_private(alice_sk_enc);
    unsigned char alice_pk_enc[32];
    ka_derive_public(alice_pk_enc, alice_sk_enc);

    auto bob_a_sk = random_bits(256);
    unsigned char bob_sk_enc[32];
    auto bob_prfed = prf_addr_sk_enc<HashT>(bob_a_sk);
    fill_with_bits(bob_sk_enc, bob_prfed);
    ka_format_private(bob_sk_enc);
    unsigned char bob_pk_enc[32];
    ka_derive_public(bob_pk_enc, bob_sk_enc);

    unsigned char alice_secret[32];
    ka_agree(alice_secret, alice_sk_enc, bob_pk_enc);

    unsigned char bob_secret[32];
    ka_agree(bob_secret, bob_sk_enc, alice_pk_enc);

    EXPECT_EQ(std::memcmp(alice_secret, bob_secret, 32), 0);
}
