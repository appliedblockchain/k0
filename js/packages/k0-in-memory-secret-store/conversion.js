const immutable = require('immutable')
const u = require('@appliedblockchain/k0-util')

function stringifyNote(a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48),
  u.checkBN(v)
  return immutable.Map({
    a_pk: u.buf2hex(a_pk),
    rho: u.buf2hex(rho),
    r: u.buf2hex(r),
    v: v.toString()
  })
}

module.exports = { stringifyNote }
