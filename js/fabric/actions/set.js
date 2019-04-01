'use strict'
const _ = require('lodash')

async function set(client, channel, chaincodeId, peers, newVal) {
  const txId = client.newTransactionID()
  const targets = _.sampleSize(peers, 2)
  console.log(`Sending proposal to ${targets.map(t => t._options['grpc.default_authority']).join(', ')}...`)
  const endorsementResults = await channel.sendTransactionProposal({
    txId: txId,
    chaincodeId,
    fcn: 'set',
		args: [ newVal ],
    targets
	})
  console.log(`Sending transaction to orderer...`)
  const proposalResponses = endorsementResults[0]
  const proposal = endorsementResults[1]
  const header = endorsementResults[2]
  const txPromise = channel.sendTransaction({
    proposalResponses,
    proposal,
    header
  })
  await txPromise
}

module.exports = set
