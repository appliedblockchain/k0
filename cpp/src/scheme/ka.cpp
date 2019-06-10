#include "ka.hpp"
#include "util.h"
#include <sodium.h>

// https://github.com/zcash/zcash/blob/v1.1.2/src/zcash/NoteEncryption.cpp
// TODO is this okay with our byte order/bit order? See ZCash extended paper
// page 58
void zktrade::clamp_curve25519(unsigned char key[32]) {
    key[0] &= 248;
    key[31] &= 127;
    key[31] |= 64;
}

void zktrade::ka_format_private(unsigned char key[32]) {
    zktrade::clamp_curve25519(key);
}

void zktrade::ka_derive_public(unsigned char pk[32],
                               const unsigned char sk_enc[32]) {
    if (crypto_scalarmult_base(pk, sk_enc) != 0) {
        throw std::logic_error("Public key derivation failed");
    }
}

void zktrade::ka_agree(unsigned char dhsecret[32], const unsigned char priv[32],
                       const unsigned char pub[32]) {
    if (crypto_scalarmult(dhsecret, priv, pub) != 0) {
        throw std::logic_error("DH secret creation failed");
    }
}
