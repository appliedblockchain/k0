'use strict'

const u = require('@appliedblockchain/k0-util')

async function handleDeposit(eventEmitter, event) {
  const [cm, newRoot] = await Promise.all([
    u.unpack(event.returnValues.cm),
    u.unpack(event.returnValues.new_root)
  ])
  await eventEmitter.emit(
    'deposit',
    u.hex2buf(event.transactionHash),
    cm,
    u.hex2buf(event.returnValues.data),
    newRoot
  )
}

module.exports = handleDeposit