'use strict'

const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function pack256Bits(jc, bits) {
  u.checkBuf(bits, 32)
  const res = await request(
    jc,
    'pack256Bits',
    [
      u.buf2hex(bits)
    ]
  )
  return res.map(u.string2bn)
}

module.exports = pack256Bits
