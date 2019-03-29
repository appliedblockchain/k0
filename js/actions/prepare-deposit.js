const BN = require('bn.js')
const crypto = require('crypto')
const u = require('../util')

async function prepareDeposit(server, platformState, secretStore, v) {
  u.checkBN(v)
  const a_pk = secretStore.getPublicKey()
  const rho = crypto.randomBytes(32)
  const r = crypto.randomBytes(48)
  const cm = await server.cm(a_pk, rho, r, v)
  console.log('got cm', cm)
  const prevRoot = await platformState.merkleTreeRoot()
  console.log('prevRoot', prevRoot)
  const mtAddSim = await platformState.simulateMerkleTreeAddition(cm)
  console.log(mtAddSim)
  const commProofData = await server.depositCommitmentProof(a_pk, rho, r, v)
  assert(cm.equals(commProofData.cm))
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
    // TODO hex2buf
    additionProof,
    commitmentProof: commProofData.proof
  }
}

module.exports = prepareDeposit
