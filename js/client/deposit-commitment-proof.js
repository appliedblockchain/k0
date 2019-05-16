const request = require('./request')
const u = require('../util')
const BN = require('bn.js')
const conv = require('./proof-conversion')

async function depositCommitmentProof(jc, a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  const res = await request(
    jc,
    'depositCommitmentProof',
    [
      u.buf2hex(a_pk),
      u.buf2hex(rho),
      u.buf2hex(r),
      v.toString()
    ]
  )
  return {
    cm: u.hex2buf(res.cm),
    k: u.hex2buf(res.k),
    proof_affine: conv.bnifyAffine(res.proof_affine),
    proof_jacobian: conv.bnifyJacobian(res.proof_jacobian)
  }
}

module.exports = depositCommitmentProof