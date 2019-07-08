#ifndef K0_SCHEME_KA_HPP
#define K0_SCHEME_KA_HPP

#include <libff/common/utils.hpp>
using namespace libff;

namespace k0 {

// Copied from Zcash
// https://github.com/zcash/zcash/blob/v1.1.2/src/zcash/NoteEncryption.cpp
void clamp_curve25519(unsigned char key[32]);
void ka_format_private(unsigned char key[32]);
void ka_derive_public(unsigned char pk[32], const unsigned char sk_enc[32]);
void ka_agree(unsigned char dhsecret[32], const unsigned char priv[32],
              const unsigned char pub[32]);

} // namespace k0

#endif // K0_SCHEME_KA_HPP
