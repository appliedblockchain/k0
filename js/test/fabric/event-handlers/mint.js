'use strict'

const u = require('../../../util')

async function handleMint(platformState, txnid, cm, nextRoot) {
  await platformState.add(txnid, [], [ cm ])
}

module.exports = handleMint
