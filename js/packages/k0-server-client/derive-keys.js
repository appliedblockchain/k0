assert = require('assert')
const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function deriveKeys(jc, a_sk) {
  u.checkBuf(a_sk, 32)
  const result = await request(jc, 'deriveKeys', [ u.buf2hex(a_sk) ])
  return {
    a_pk: u.hex2buf(result.a_pk),
    sk_enc: u.hex2buf(result.sk_enc),
    pk_enc: u.hex2buf(result.pk_enc)
  }
}

module.exports = deriveKeys
