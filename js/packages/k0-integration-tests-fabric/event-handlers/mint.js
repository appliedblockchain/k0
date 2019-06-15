'use strict'

const u = require('@appliedblockchain/k0-util')

async function handleMint(
  platformState,
  secretStore,
  k0,
  txnid,
  cm,
  data,
  nextRoot
) {
  const decryptionResult = await k0.decryptNote(secretStore, data)
  if (decryptionResult.success) {
    secretStore.addNoteInfo(
      cm,
      secretStore.getAPk(),
      decryptionResult.rho,
      decryptionResult.r,
      decryptionResult.v
    )
  }
  await platformState.add(txnid, [], [ cm ], nextRoot)
}

module.exports = handleMint
