'use strict'

const u = require('../../../util')

async function handleMint(
  platformState,
  secretStore,
  k0,
  txnid,
  cm,
  data,
  nextRoot
) {

  console.inspect({
    label: `INCOMMMING MINT TX id: ${txnid}`,
    date: (new Date()).getTime()
  })

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
