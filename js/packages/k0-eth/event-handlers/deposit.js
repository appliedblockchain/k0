'use strict'

const BN = require('bn.js')
const u = require('@appliedblockchain/k0-util')

async function handleDeposit(server, eventEmitter, event) {
  const [cm, newRoot] = await Promise.all([
    server.unpack256Bits(
      new BN(event.returnValues.cm[0].toString()),
      new BN(event.returnValues.cm[1].toString())
    ),
    server.unpack256Bits(
      new BN(event.returnValues.new_root[0].toString()),
      new BN(event.returnValues.new_root[1].toString())
    )
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
