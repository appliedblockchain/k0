'use strict'

const BN = require('bn.js')

// Return the current number of leaves and the current root from a queryPeer
async function getState(
  logger,
  channel,
  queryPeer,
  args = []
) {

  const [ buf ] = await channel.queryByChaincode({
    chaincodeId: process.env.CHAINCODE_ID,
    fcn: 'getState',
    args,
    targets: [ queryPeer ]
  })

  if (!buf) {
    throw new Error('Cannot get state from peer:', queryPeer)
  }

  const root = buf.slice(0, 32)
  const numLeaves = new BN(buf.slice(32, 40).toString('hex'), 'hex', 'le')

  return {
    root,
    numLeaves
  }
}

module.exports = getState
