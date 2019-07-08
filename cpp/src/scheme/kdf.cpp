#include "kdf.hpp"
#include "util.h"
#include <sodium.h>

// Adapded from ZCash

void k0::kdf(unsigned char key[32], const unsigned char dhsecret[32],
                  const unsigned char epk[32], const unsigned char pk_enc[32]) {

    unsigned char block[96] = {};
    memcpy(block + 0, dhsecret, 32);
    memcpy(block + 32, epk, 32);
    memcpy(block + 64, pk_enc, 32);

    unsigned char personalization[crypto_generichash_blake2b_PERSONALBYTES] =
        {};
    memcpy(personalization, "K0Cash", 6);

    if (crypto_generichash_blake2b_salt_personal(key, 32, block, 96, NULL,
                                                 0,    // No key.
                                                 NULL, // No salt.
                                                 personalization) != 0) {
        throw std::logic_error("KDF failed");
    }
}
