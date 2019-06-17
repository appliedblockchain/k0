'use strict'

const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function unpack256Bits(jc, a, b) {
  u.checkBN(a)
  u.checkBN(b)
  const res = await request(
    jc,
    'unpack256Bits',
    [ a, b ].map(u.string2bn)
  )
  return u.hex2buf(res)
}

module.exports = unpack256Bits
