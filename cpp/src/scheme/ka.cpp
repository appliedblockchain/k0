#include <sodium.h>
#include "ka.hpp"
#include "util.h"

// https://github.com/zcash/zcash/blob/v1.1.2/src/zcash/NoteEncryption.cpp
// TODO is this okay with our byte order/bit order? See ZCash extended paper page 58
void zktrade::clamp_curve25519(unsigned char key[32])
{
    key[0] &= 248;
    key[31] &= 127;
    key[31] |= 64;
}

bit_vector zktrade::ka_format_private(bit_vector input)
{
    assert(input.size() == 256);
    auto bytes = bits_to_bytes(input);
    clamp_curve25519(&bytes[0]);
    return bytes_to_bits(bytes);
}

bit_vector zktrade::ka_derive_public(bit_vector input)
{
    assert(input.size() == 256);
    auto sk_enc = bits_to_bytes(input);
    unsigned char pk[32];
    if (crypto_scalarmult_base(pk, &sk_enc[0]) != 0) {
        throw std::logic_error("Public key derivation failed");
    }
    vector<unsigned char> pk_vec(pk, pk+32);
    return bytes_to_bits(pk_vec);
}

bit_vector zktrade::ka_agree(bit_vector priv, bit_vector pub)
{
    assert(priv.size() == 32);
    assert(pub.size() == 32);

    auto privbytes = bits_to_bytes(priv);
    auto pubbytes = bits_to_bytes(pub);
    unsigned char secret[32];
    if (crypto_scalarmult(&secret[0], &privbytes[0], &pubbytes[0]) != 0) {
        throw std::logic_error("DH secret creation failed");
    }
    vector<unsigned char> secret_vec(secret, secret+32);
    return bytes_to_bits(secret_vec);
}
