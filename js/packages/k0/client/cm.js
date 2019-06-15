const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function cm(jc, a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  const res = await request(
    jc,
    'cm',
    [
      u.buf2hex(a_pk),
      u.buf2hex(rho),
      u.buf2hex(r),
      v.toString()
    ]
  )
  return u.hex2buf(res)
}

module.exports = cm
