#ifndef ZKTRADE_SCHEME_KA_HPP
#define ZKTRADE_SCHEME_KA_HPP

#include <libff/common/utils.hpp>
using namespace libff;

namespace zktrade {

    // Copied from Zcash
    // https://github.com/zcash/zcash/blob/v1.1.2/src/zcash/NoteEncryption.cpp
    void clamp_curve25519(unsigned char key[32]);

    bit_vector ka_format_private(bit_vector input);
    bit_vector ka_derive_public(bit_vector input);
    bit_vector ka_agree(bit_vector priv, bit_vector pub);

}

#endif //ZKTRADE_SCHEME_KA_HPP
