'use strict'

const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function decryptNote(jc, ciphertext, sk_enc, pk_enc) {
  u.checkBuf(ciphertext, 136)
  u.checkBuf(sk_enc, 32)
  u.checkBuf(pk_enc, 32)
  const res = await request(
    jc,
    'decrypt_note',
    [
      u.buf2hex(ciphertext),
      u.buf2hex(sk_enc),
      u.buf2hex(pk_enc)
    ]
  )

  if (res.success) {
    return {
      success: true,
      value: u.hex2buf(res.value)
    }
  } else {
    return {
      success: false
    }
  }
}

module.exports = decryptNote
