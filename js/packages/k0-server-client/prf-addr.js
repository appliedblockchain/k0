assert = require('assert')
const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function a_pk(jc, a_sk) {
  u.checkBuf(a_sk, 32)
  const result = await request(jc, 'prf_addr', [ u.buf2hex(a_sk) ])
  return u.hex2buf(result)
}

module.exports = a_pk
