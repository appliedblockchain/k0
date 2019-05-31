#include <gtest/gtest.h>
#include <libff/common/utils.hpp>
#include <sodium.h>
#include "serialization.hpp"
#include "util.h"

using namespace std;
using namespace libff;
using namespace zktrade;

TEST(Encryption, Encryption) {

    ASSERT_FALSE(sodium_init());

    auto input = random_bits(256);

    vector<unsigned char> message = bits_to_bytes(input);
    unsigned char* message_ptr = &message[0];
    auto message_len = 32;

    unsigned char nonce[crypto_aead_chacha20poly1305_IETF_NPUBBYTES];
    unsigned char key[crypto_aead_chacha20poly1305_IETF_KEYBYTES];
    unsigned char
        ciphertext[message.size() + crypto_aead_chacha20poly1305_IETF_ABYTES];
    unsigned long long ciphertext_len;

    crypto_aead_chacha20poly1305_ietf_keygen(key);
    randombytes_buf(nonce, sizeof nonce);

    crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext, &ciphertext_len, message_ptr, message_len,
        NULL, 0, NULL, nonce, key);

    unsigned char decrypted[message_len];
    unsigned long long decrypted_len;
    ASSERT_EQ(
        crypto_aead_chacha20poly1305_ietf_decrypt(
            decrypted, &decrypted_len, NULL, ciphertext, ciphertext_len, NULL,
            0, nonce, key),
        0);
    vector<unsigned char> decrypted_v(decrypted, decrypted + decrypted_len);
    bit_vector decrypted_bv = bytes_to_bits(decrypted_v);
    ASSERT_EQ(decrypted_bv, input);
}
