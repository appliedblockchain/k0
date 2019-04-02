'use strict'
const _ = require('lodash')

async function sendTransaction(client, channel, chaincodeId, peers, fcn, params) {
  const txId = client.newTransactionID()
  const targets = _.sampleSize(peers, 2)
  console.log(`Sending proposal to ${targets.map(t => t._options['grpc.default_authority']).join(', ')}...`)
  const endorsementResults = await channel.sendTransactionProposal({
    txId: txId,
    chaincodeId,
    fcn,
    args: params,
    targets
	})
  console.log(`Sending transaction to orderer...`)
  const proposalResponses = endorsementResults[0]
  const proposal = endorsementResults[1]
  const header = endorsementResults[2]
  const result = await channel.sendTransaction({
    proposalResponses,
    proposal,
    header
  })
  assert(result.status === 'SUCCESS')
}

module.exports = sendTransaction
