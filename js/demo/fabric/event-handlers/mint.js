const chalk = require('chalk')
const u = require('../../../util')

async function handleMint(platformState, txnid, cm, nextRoot) {
  await platformState.add(txnid, [], [ u.hex2buf(cm) ])
  console.log(chalk.grey('DEPOSIT'))
  console.log(chalk.grey(`CM ${cm}`))
  console.log(chalk.grey(`New Merkle tree root ${nextRoot}`))
}

module.exports = handleMint
