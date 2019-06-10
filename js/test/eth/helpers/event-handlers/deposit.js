'use strict'

const chalk = require('chalk')
const decodeData = require('./decode-data')
const u = require('../../../../util')

async function handleDeposit(platformState, secretStore, emitter, txHash, cm,
                             data, nextRoot) {
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
  await platformState.add(u.buf2hex(txHash), [], [ cm ], nextRoot)
  emitter.emit('depositProcessed', txHash)
  console.log(chalk.grey('DEPOSIT'))
  console.log(chalk.grey(`CM ${u.buf2hex(cm)}`))
  console.log(chalk.grey(`New Merkle tree root ${u.buf2hex(nextRoot)}`))
}

module.exports = handleDeposit
