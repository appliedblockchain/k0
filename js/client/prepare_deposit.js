const request = require('./request')
const u = require('../util')
const BN = require('bn.js')

function prepare_deposit(jc, a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  return request(
    jc,
    'prepare_deposit',
    [
      u.buf2hex(a_pk),
      u.buf2hex(rho),
      u.buf2hex(r),
      v.toString()
    ]
  )
}

module.exports = prepare_deposit