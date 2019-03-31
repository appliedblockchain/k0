const u = require('../../util')
const logger = require('../../logger')

async function handleDeposit(platformState, txHash, cm, nextRoot) {
  console.log('((((((((((( DEMO DEPOSIT )))))))))))')
  await platformState.add(u.buf2hex(txHash), [], [ cm ], nextRoot)
  logger.info(
    `Deposit: CM ${u.buf2hex(cm)}, new Merkle tree root ${u.buf2hex(nextRoot)}`
  )
}

module.exports = handleDeposit
