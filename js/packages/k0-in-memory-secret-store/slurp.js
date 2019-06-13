const u = require('@appliedblockchain/k0-util')
const BN = require('bn.js')
const Immutable = require('immutable')
const conv = require('./conversion')

function slurp(obj) {
  let cms = Immutable.Map()
  const keys = Object.keys(obj.cms || {})
  for (let i = 0; i < keys.length; i++) {
    const cmHex = keys[i]
    const { a_pk, rho, r, v } = obj.cms[cmHex]
    cms = cms.set(cmHex, conv.stringifyNote(
      u.hex2buf(a_pk),
      u.hex2buf(rho),
      u.hex2buf(r),
      new BN(v)
    ))
  }
  const privateKey = u.hex2buf(obj.privateKey)
  const publicKey = u.hex2buf(obj.publicKey)
  u.checkBuf(privateKey, 32)
  u.checkBuf(publicKey, 32)
  return Immutable.Map({
    privateKey: u.buf2hex(privateKey),
    publicKey: u.buf2hex(publicKey),
    cms: cms
  })
}

module.exports = slurp
