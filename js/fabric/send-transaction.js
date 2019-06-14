'use strict'
const _ = require('lodash')
const assert = require('assert')
const waitForTx = require('./wait-for-tx')

async function sendTransaction(
  logger,
  client,
  channel,
  chaincodeId,
  peers,
  queryPeer,
  fcn,
  params
) {
  const txId = client.newTransactionID()
  const numTargets = 2
  const targets = _.sampleSize(peers, numTargets)
  logger.debug([
    'Sending proposal to ',
    targets.map(t => {
      let text = t._url
      if (t._options['grpc.default_authority'] !== undefined) {
        text += " " + t._options['grpc.default_authority']
      }
      return text
    }).join(', '),
    '...'
  ].join(''))

  const endorsementResults = await channel.sendTransactionProposal({
    txId: txId,
    chaincodeId,
    fcn,
    args: params,
    targets
  })

  console.inspect({ label: 'EndorsmentResult', endorsementResults })

  const [ proposalResponses, proposal, header ] = endorsementResults
  let allEndorsed = true
  for (let i = 0; i < proposalResponses.length; i = i + 1) {
    const r = proposalResponses[i]
    if (!(r.response && r.response.status === 200)) {
      allEndorsed = false
      break
    }
  }
  console.inspect({ label: `SUCCESS on send tx id ${txId._transaction_id}`, date: (new Date()).getTime() })
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

  console.inspect('Send Transaction Result', { result })

  assert(result.status === 'SUCCESS')



  return {
    transactionId: txId._transaction_id,
    nonce: txId._nonce

  }
}

module.exports = sendTransaction
