#include "scheme/note_encryption.hpp"

template <typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
string zktrade::Server<FieldT, CommitmentHashT, MerkleTreeHashT>::encrypt_note(
    const string& plaintext_hex_str, const string& pk_enc_hex_str)
{
    if (plaintext_hex_str.length() != 2 + 88 * 2) {
        throw JsonRpcException(-32602, "Invalid plaintext length");
    }
    if (plaintext_hex_str.length() != 2 + 32 * 2) {
        throw JsonRpcException(-32602, "Invalid pk_enc length");
    }

    unsigned char plaintext[88];
    fill_with_bytes_of_hex_string(plaintext, plaintext_hex_str);
    unsigned char pk_enc[32];
    fill_with_bytes_of_hex_string(pk_enc, pk_enc_hex_str);

    unsigned char ciphertext[104];
    if (zktrade::encrypt_note(ciphertext, plaintext, pk_enc) == 0) {
        return bytes_to_hex(ciphertext, 32);
    } else {
        throw JsonRpcException(-32010, "Encryption failed.");
    }
}
