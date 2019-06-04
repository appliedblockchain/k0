'use strict'
const _ = require('lodash')
const assert = require('assert')
const waitForTx = require('./wait-for-tx')

async function sendTransaction(logger, client, channel, chaincodeId, peers,
  queryPeer, fcn, params) {
  const txId = client.newTransactionID()
  const numTargets = 2
  const targets = _.sampleSize(peers, numTargets)
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
  let allEndorsed = true
  for (let i = 0; i < proposalResponses.length; i = i + 1) {
    const r = proposalResponses[i]
    if (!(r.response && r.response.status === 200)) {
      allEndorsed = false
      break
    }
  }
  if (!allEndorsed) {
    proposalResponses.forEach((r, i) => console.log(i, r))
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
