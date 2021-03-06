#ifndef K0_SCHEME_NOTE_ENCRYPTION_HPP
#define K0_SCHEME_NOTE_ENCRYPTION_HPP

#include <libff/common/utils.hpp>
using namespace libff;

namespace k0 {

int encrypt_note(unsigned char epk[32],
                 unsigned char ciphertext[104],
                 const unsigned char plaintext[88],
                 const unsigned char pk_enc[32]);

int decrypt_note(unsigned char plaintext[88],
                 const unsigned char epk[32],
                 const unsigned char ciphertext[104],
                 const unsigned char sk_enc[32],
                 const unsigned char pk_enc[32]);

}

#endif // K0_SCHEME_NOTE_ENCRYPTION_HPP
