const request = require('./request')
const u = require('../util')
const BN = require('bn.js')

async function prepare_deposit(jc, a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  const res = await request(
    jc,
    'prepare_deposit',
    [
      u.buf2hex(a_pk),
      u.buf2hex(rho),
      u.buf2hex(r),
      v.toString()
    ]
  )
  return {
    address: new BN(res.address),
    cm: u.hex2buf(res.cm),
    k: u.hex2buf(res.k),
    nextRoot: u.hex2buf(res.nextRoot),
    commitmentProof: res.commitmentProof,
    additionProof: res.additionProof
  }
}

module.exports = prepare_deposit