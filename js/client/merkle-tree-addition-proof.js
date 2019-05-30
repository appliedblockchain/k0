const request = require('./request')
const u = require('../util')
const BN = require('bn.js')
const conv = require('../proof-conversion')

async function merkleTreeAdditionProof(
  jc, prevRoot, address, leaf, path, nextRoot) {
  u.checkBuf(prevRoot, 32)
  u.checkBN(address)
  u.checkBuf(leaf, 32)
  path.forEach(x => u.checkBuf(x, 32))
  u.checkBuf(nextRoot, 32)
  const res = await request(
    jc,
    'merkleTreeAdditionProof',
    [
      u.buf2hex(prevRoot),
      address.toString(),
      u.buf2hex(leaf),
      path.map(x => u.buf2hex(x)),
      u.buf2hex(nextRoot)
    ]
  )
  return {
    proof_affine: conv.bnifyAffine(res.proof_affine),
    proof_jacobian: conv.bnifyJacobian(res.proof_jacobian)
  }
}

module.exports = merkleTreeAdditionProof
