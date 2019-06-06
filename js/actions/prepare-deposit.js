
const crypto = require('crypto')
const u = require('../util')

async function prepareDeposit(server, platformState, a_pk, v) {
  u.checkBN(v)
  u.checkBuf(a_pk, 32)

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
  return {
    a_pk,
    rho,
    r,
    cm,
    k: commProofData.k,
    nextRoot: mtAddSim.nextRoot,
    additionProofAffine: additionProof.proof_affine,
    additionProofJacobian: additionProof.proof_jacobian,
    commitmentProofAffine: commProofData.proof_affine,
    commitmentProofJacobian: commProofData.proof_jacobian
  }
}

module.exports = prepareDeposit
