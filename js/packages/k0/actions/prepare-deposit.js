'use strict'

const crypto = require('crypto')
const encryptNote = require('./helpers/encrypt-note')
const u = require('@appliedblockchain/k0-util')

async function prepareDeposit(server, platformState, address, v) {
  u.checkBuf(address, 64)
  u.checkBN(v)

  const a_pk = address.slice(0, 32)
  const pk_enc = address.slice(32, 64)

  const rho = crypto.randomBytes(32)
  const r = crypto.randomBytes(48)

  const cm = await server.cm(a_pk, rho, r, v)
  const prevRoot = await platformState.merkleTreeRoot()
  const mtAddSim = await platformState.simulateMerkleTreeAddition(cm)
  const commProofData = await server.depositCommitmentProof(a_pk, rho, r, v)
  const additionProof = await server.merkleTreeAdditionProof(
    prevRoot,
    mtAddSim.address,
    cm,
    mtAddSim.path,
    mtAddSim.nextRoot
  )

  const ciphertext = await encryptNote(server, address, v, rho, r)
  return {
    a_pk,
    rho,
    r,
    cm,
    ciphertext,
    k: commProofData.k,
    nextRoot: mtAddSim.nextRoot,
    additionProofAffine: additionProof.proof_affine,
    additionProofJacobian: additionProof.proof_jacobian,
    commitmentProofAffine: commProofData.proof_affine,
    commitmentProofJacobian: commProofData.proof_jacobian
  }
}

module.exports = prepareDeposit
