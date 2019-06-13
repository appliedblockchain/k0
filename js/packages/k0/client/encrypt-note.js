'use strict'

const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function encryptNote(jc, plaintext, pk_enc) {
  u.checkBuf(plaintext, 88)
  u.checkBuf(pk_enc, 32)
  const res = await request(
    jc,
    'encrypt_note',
    [
      u.buf2hex(plaintext),
      u.buf2hex(pk_enc)
    ]
  )

  return u.hex2buf(res)
}

module.exports = encryptNote
