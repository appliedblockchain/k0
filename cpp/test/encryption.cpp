#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/gadgets/sha256_compression.h"
#include "scheme/ka.hpp"
#include "scheme/kdf.hpp"
#include "scheme/prfs.h"
#include "scheme/note_encryption.hpp"
#include "serialization.hpp"
#include "util.h"
#include <libff/common/utils.hpp>
#include <sodium.h>

using namespace std;
using namespace libff;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_compression_gadget<FieldT> HashT;

TEST(Encryption, Functions) {

    EXPECT_NE(sodium_init(), -1);
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

    auto carol_a_sk = random_bits(256);
    unsigned char carol_sk_enc[32];
    auto carol_prfed = prf_addr_sk_enc<HashT>(carol_a_sk);
    fill_with_bits(carol_sk_enc, carol_prfed);
    ka_format_private(carol_sk_enc);
    unsigned char carol_pk_enc[32];
    ka_derive_public(carol_pk_enc, carol_sk_enc);

    unsigned char message[32];
    fill_with_random_bytes(message, 32);

    unsigned char ciphertext[80];

    EXPECT_EQ(encrypt_note(ciphertext, message, bob_pk_enc), 0);

    unsigned char decrypted_text[32];
    EXPECT_EQ(decrypt_note(decrypted_text, ciphertext, carol_sk_enc,
                           carol_pk_enc),
              -1);
    EXPECT_EQ(decrypt_note(decrypted_text, ciphertext, bob_sk_enc,
                           bob_pk_enc),
              0);

    EXPECT_EQ(std::memcmp(decrypted_text, message, 32), 0);

}

TEST(Encryption, Steps) {

    EXPECT_NE(sodium_init(), -1);

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

    auto carol_a_sk = random_bits(256);
    unsigned char carol_sk_enc[32];
    auto carol_prfed = prf_addr_sk_enc<HashT>(carol_a_sk);
    fill_with_bits(carol_sk_enc, carol_prfed);
    ka_format_private(carol_sk_enc);
    unsigned char carol_pk_enc[32];
    ka_derive_public(carol_pk_enc, carol_sk_enc);

    // ENCRYPTION

    unsigned char esk[32];
    fill_with_random_bytes(esk, 32);
    unsigned char epk[32];
    ka_derive_public(epk, esk);

    unsigned char alice_dhsecret[32];
    ka_agree(alice_dhsecret, esk, bob_pk_enc);

    unsigned char encryption_key[32];
    kdf(encryption_key, alice_dhsecret, epk, bob_pk_enc);

    // The nonce is zero because we never reuse keys
    constexpr auto enc_cipher_length =
        crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
    unsigned char encryption_cipher_nonce[enc_cipher_length] {};

    unsigned long long message_length = 32;
    unsigned char message[message_length];
    fill_with_random_bytes(message, message_length);


    auto max_ciphertext_length = message_length +
        crypto_aead_chacha20poly1305_IETF_ABYTES;
    unsigned char ciphertext[max_ciphertext_length];
    unsigned long long ciphertext_length;

    EXPECT_EQ(crypto_aead_chacha20poly1305_ietf_encrypt(
                  ciphertext, &ciphertext_length, message, message_length, NULL,
                  0, NULL, encryption_cipher_nonce, encryption_key),
              0);

    // DECRYPTION

    unsigned char bob_dhsecret[32];
    ka_agree(bob_dhsecret, bob_sk_enc, epk);

    unsigned char carol_dhsecret[32];
    ka_agree(carol_dhsecret, carol_sk_enc, epk);

    // Alice and Bob should have the same DH secret
    EXPECT_EQ(std::memcmp(bob_dhsecret, alice_dhsecret, 32), 0);

    // Carol should come to a different result
    EXPECT_NE(std::memcmp(carol_dhsecret, alice_dhsecret, 32), 0);

    unsigned char bob_decryption_key[32];
    kdf(bob_decryption_key, bob_dhsecret, epk, bob_pk_enc);
    // Key should be equal to the one derived during encryption
    EXPECT_EQ(std::memcmp(bob_decryption_key, encryption_key, 32), 0);

    unsigned char carol_decryption_key[32];
    kdf(carol_decryption_key, carol_dhsecret, epk, carol_pk_enc);
    EXPECT_NE(std::memcmp(carol_decryption_key, encryption_key, 32), 0);

    auto max_decrypted_text_length = ciphertext_length -
        crypto_aead_chacha20poly1305_ABYTES;
    unsigned char decrypted_text[max_decrypted_text_length];
    unsigned long long decrypted_text_length;

    // The nonce is zero because we never reuse keys
    constexpr auto nonce_length = crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
    unsigned char decryption_cipher_nonce[nonce_length] = {};

    EXPECT_EQ(crypto_aead_chacha20poly1305_ietf_decrypt(
                  decrypted_text,
                  &decrypted_text_length,
                  NULL,
                  ciphertext,
                  ciphertext_length,
                  NULL,
                  0,
                  decryption_cipher_nonce,
                  bob_decryption_key), 0);
    EXPECT_EQ(decrypted_text_length, message_length);
    EXPECT_EQ(std::memcmp(decrypted_text, message, 32), 0);

    // Carol's decryption attempt should fail
    EXPECT_EQ(crypto_aead_chacha20poly1305_ietf_decrypt(
                  decrypted_text,
                  &decrypted_text_length,
                  NULL,
                  ciphertext,
                  ciphertext_length,
                  NULL,
                  0,
                  decryption_cipher_nonce,
                  carol_decryption_key), -1);

}
