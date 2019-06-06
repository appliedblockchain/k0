#ifndef ZKTRADE_SCHEME_NOTE_ENCRYPTION_HPP
#define ZKTRADE_SCHEME_NOTE_ENCRYPTION_HPP

#include <libff/common/utils.hpp>
using namespace libff;

namespace zktrade {

// TODO concat ciphertext+epk
int encrypt_note(unsigned char ciphertext[48],
                 unsigned char epk[32],
                 const unsigned char message[32],
                 const unsigned char pk_enc[32]);

int decrypt_note(unsigned char decrypted_text[32],
                 const unsigned char ciphertext[48],
                 const unsigned char epk[32],
                 const unsigned char sk_enc[32],
                 const unsigned char pk_enc[32]);

}

#endif // ZKTRADE_SCHEME_NOTE_ENCRYPTION_HPP
