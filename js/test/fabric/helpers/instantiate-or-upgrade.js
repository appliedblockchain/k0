'use strict'

const waitForTx = require('./wait-for-tx')

async function instantiateOrUpgrade(
  clientBundle,
  endorsementPolicy,
  chaincodeId,
  version,
  args,
  upgrade = false
) {
  const { client, channel, queryPeer } = clientBundle


  const eh = channel.newChannelEventHub(queryPeer)
  eh.connect()

  const tx_id = client.newTransactionID()

  const request = {
    chaincodeId: chaincodeId,
    chaincodeVersion: version,
    args: args,
    txId: tx_id,
    'endorsement-policy': endorsementPolicy,
    targets: [ queryPeer ]
  }

  let endorsementResults
  if (upgrade) {
    endorsementResults = await channel.sendUpgradeProposal(request)
  } else {
    endorsementResults = await channel.sendInstantiateProposal(request)
  }

  const proposalResponses = endorsementResults[0]
  const proposal = endorsementResults[1]
  const header = endorsementResults[2]

  if (proposalResponses
      && proposalResponses[0].response
      && proposalResponses[0].response.status === 200) {
    // everything ok
    console.inspect('ALL OK', proposalResponses[0])
  } else {
    console.log(proposalResponses[0])
    eh.close()
    throw new Error('Proposal was bad')
  }
  const tx = {
    proposalResponses: proposalResponses,
    proposal: proposal,
    header: header
  }
  await waitForTx(eh, tx_id._transaction_id, channel.sendTransaction(tx))
  eh.close()
}

module.exports = instantiateOrUpgrade
