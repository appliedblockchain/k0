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

TEST(KA, clamp) {
    unsigned char input[80];
    cout << "INPUT BEFORE ";
    for (size_t i = 0; i < 80; i++) {
        cout << (int) input[i] << " ";
    }
    cout << endl;
    fill_with_random_bytes(input, 32);
    cout << "INPUT AFTER  " << endl;
    for (size_t i = 0; i < 80; i++) {
        cout << (int) input[i] << " ";
    }
    cout << endl;
}

TEST(KA, FormatPrivate) {
    auto a_sk = random_bits(256);
    unsigned char sk_enc[32];
    auto prfed = prf_addr_sk_enc<HashT>(a_sk);
    fill_with_bits(sk_enc, prfed);
    ka_format_private(sk_enc);
}

TEST(KA, DerivePublic) {
    auto a_sk = random_bits(256);
    unsigned char sk_enc[32];
    auto prfed = prf_addr_sk_enc<HashT>(a_sk);
    fill_with_bits(sk_enc, prfed);
    ka_format_private(sk_enc);

    unsigned char pk_enc[32];
    ka_derive_public(pk_enc, sk_enc);
}

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

    for (size_t i = 0; i < 32; i++) {
        ASSERT_EQ(alice_secret[i], bob_secret[i]);
    }
}
