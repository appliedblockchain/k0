const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function prepare_withdrawal(jc, address, a_sk, rho, r, v, recipient) {
  u.checkBN(address)
  u.checkBuf(a_sk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  u.checkBuf(recipient, 20)
  const res = await request(
    jc,
    'prepare_withdrawal',
    [
      address.toString(),
      u.buf2hex(a_sk),
      u.buf2hex(rho),
      u.buf2hex(r),
      v.toString(),
      u.buf2hex(recipient)
    ]
  )
  return {
    sn: u.hex2buf(res.sn),
    // TODO: BNify
    proof: res.proof
  }
}

module.exports = prepare_withdrawal