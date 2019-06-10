'use strict'

const decodeData = require('./decode-data')
const u = require('../../../util')

async function handleMint(platformState, secretStore, txnid, cm, data,
                          nextRoot) {
  console.log('got data', data)
  const info = decodeData(data)
  console.log('info pubkey       ', info.a_pk)
  console.log('secretstore pubkey', secretStore.getPublicKey())
  if (info.a_pk.equals(secretStore.getPublicKey())) {
    console.log('adding to secretstore')
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
