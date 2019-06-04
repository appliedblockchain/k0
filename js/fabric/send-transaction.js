'use strict'
const _ = require('lodash')
const assert = require('assert')
const waitForTx = require('./wait-for-tx')

async function sendTransaction(logger, client, channel, chaincodeId, peers,
  queryPeer, fcn, params) {
  const txId = client.newTransactionID()
  const targets = _.sampleSize(peers, 2)
  logger.debug([
    'Sending proposal to ',
    targets.map(t => t._options['grpc.default_authority']).join(', '),
    '...'
  ].join(''))

  const endorsementResults = await channel.sendTransactionProposal({
    txId: txId,
    chaincodeId,
    fcn,
    args: params,
    targets
  })

  const [ proposalResponses, proposal, header ] = endorsementResults
  if (!(proposalResponses[0].response
        && proposalResponses[0].response.status === 200
        && proposalResponses[1].response
        && proposalResponses[1].response.status === 200)) {
    console.error(JSON.stringify(endorsementResults))
    console.inspect(proposalResponses[0])
    throw new Error('At least one endorsement failed')
  }

  logger.debug('Sending transaction to orderer...')
  const result = await waitForTx(
    channel,
    queryPeer,
    txId._transaction_id,
    channel.sendTransaction({ proposalResponses, proposal, header })
  )
  assert(result.status === 'SUCCESS')
}

module.exports = sendTransaction
