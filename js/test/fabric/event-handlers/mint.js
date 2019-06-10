'use strict'

const decodeData = require('./decode-data')
const u = require('../../../util')

async function handleMint(platformState, secretStore, txnid, cm, data,
                          nextRoot) {
  const info = decodeData(data)
  if (info.a_pk.equals(secretStore.getPublicKey())) {
    secretStore.addNoteInfo(
      cm,
      info.a_pk,
      info.rho,
      info.r,
      info.v
    )
  }
  await platformState.add(txnid, [], [ cm ], nextRoot)
}

module.exports = handleMint
