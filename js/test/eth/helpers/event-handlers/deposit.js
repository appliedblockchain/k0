'use strict'

const chalk = require('chalk')
const u = require('../../../../util')

async function handleDeposit(platformState, emitter, txHash, cm, nextRoot) {
  await platformState.add(u.buf2hex(txHash), [], [ cm ], nextRoot)
  emitter.emit('depositProcessed', txHash)
  console.log(chalk.grey('DEPOSIT'))
  console.log(chalk.grey(`CM ${u.buf2hex(cm)}`))
  console.log(chalk.grey(`New Merkle tree root ${u.buf2hex(nextRoot)}`))
}

module.exports = handleDeposit
