#include <sodium.h>
#include "note_encryption.hpp"
#include "scheme/ka.hpp"
#include "scheme/kdf.hpp"
#include "util.h"

int zktrade::encrypt_note(unsigned char ciphertext[104],
                          const unsigned char plaintext[88],
                          const unsigned char pk_enc[32])
{
    if (sodium_init() == -1) {
        throw std::logic_error("libsodium init failed");
    }
    unsigned char esk[32];
    fill_with_random_bytes(esk, 32);

    // Derive DH secret from epk and esk. epk is first 32 bits of ciphertext
    ka_derive_public(ciphertext, esk);

    unsigned char dhsecret[32];
    ka_agree(dhsecret, esk, pk_enc);

    unsigned char key[32];
    // epk: first 32 bits of ciphertext
    kdf(key, dhsecret, ciphertext, pk_enc);

    const auto nonce_len = crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
    unsigned char cipher_nonce[nonce_len] = {};

    // actual ciphertext: ciphertext from 33rd byte (first 32 are epk)
    return crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext+32, NULL, plaintext, 88, NULL, 0, NULL, cipher_nonce, key);
}

int zktrade::decrypt_note(unsigned char plaintext[88],
                          const unsigned char ciphertext[104],
                          const unsigned char sk_enc[32],
                          const unsigned char pk_enc[32])
{
    if (sodium_init() == -1) {
        throw std::logic_error("libsodium init failed");
    }

    unsigned char dhsecret[32];

    // Derive DH secret from sk_enc and epk. epk is first 32 bits of ciphertext
    ka_agree(dhsecret, sk_enc, ciphertext);

    unsigned char key[32];
    // epk: first 32 bits of ciphertext
    kdf(key, dhsecret, ciphertext, pk_enc);

    const auto nonce_len = crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
    unsigned char cipher_nonce[nonce_len] = {};

    // actual ciphertext: ciphertext from 33rd byte (first 32 are epk)
    return crypto_aead_chacha20poly1305_ietf_decrypt(
        plaintext, NULL, NULL, ciphertext+32, 104, NULL, 0, cipher_nonce, key);
}
