#include <sodium.h>
#include "note_encryption.hpp"
#include "scheme/ka.hpp"
#include "scheme/kdf.hpp"
#include "util.h"

int zktrade::encrypt_note(unsigned char ciphertext[48],
                          unsigned char epk[32],
                          const unsigned char message[32],
                          const unsigned char pk_enc[32])
{
    if (sodium_init() == -1) {
        throw std::logic_error("libsodium init failed");
    }
    unsigned char esk[32];
    fill_with_random_bytes(esk, 32);
    ka_derive_public(epk, esk);

    unsigned char dhsecret[32];
    ka_agree(dhsecret, esk, pk_enc);

    unsigned char key[32];
    kdf(key, dhsecret, epk, pk_enc);

    const auto nonce_len = crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
    unsigned char cipher_nonce[nonce_len] = {};

    return crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext, &ciphertext_length, message, 32, NULL, 0, NULL,
        cipher_nonce, key);
}

int zktrade::decrypt_note(unsigned char decrypted_text[32],
                          const unsigned char ciphertext[48],
                          const unsigned char epk[32],
                          const unsigned char sk_enc[32],
                          const unsigned char pk_enc[32])
{
    if (sodium_init() == -1) {
        throw std::logic_error("libsodium init failed");
    }

    unsigned char dhsecret[32];
    ka_agree(dhsecret, sk_enc, epk);

    unsigned char key[32];
    kdf(key, dhsecret, epk, pk_enc);

    const auto nonce_len = crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
    unsigned char cipher_nonce[nonce_len] = {};

    return crypto_aead_chacha20poly1305_ietf_decrypt(
        decrypted_text, NULL, NULL, ciphertext, 48, NULL, 0, cipher_nonce, key);
}
