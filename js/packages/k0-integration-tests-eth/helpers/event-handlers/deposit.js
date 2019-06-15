'use strict'

const u = require('@appliedblockchain/k0-util')

async function handleDeposit(platformState, secretStore, k0, emitter, txHash,
                             cm, data, nextRoot) {
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
  await platformState.add(u.buf2hex(txHash), [], [ cm ], nextRoot)
  emitter.emit('depositProcessed', txHash)
}

module.exports = handleDeposit
