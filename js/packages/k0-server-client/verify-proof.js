'use strict'

const conv = require('@appliedblockchain/k0-util/proof-conversion')
const request = require('./request')
const u = require('@appliedblockchain/k0-util')

const proofNames = [
  'commitment',
  'addition',
  'transfer',
  'withdrawal'
]

async function verifyProof(jc, what, proof, publicInputs) {
  assert(proofNames.indexOf(what) !== -1)
  u.checkProofJacobian(proof)
  publicInputs.forEach(u.checkBN)
  return request(
    jc,
    'verifyProof',
    [
      what,
      conv.stringifyJacobian(proof),
      publicInputs.map(u.bn2string)
    ]
  )
}

module.exports = verifyProof
