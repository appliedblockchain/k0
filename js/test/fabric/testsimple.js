'use strict'

const getConfig = require('./helpers/get-config')
const makeClient = require('../../fabric/client')
const sendTransaction = require('../../fabric/send-transaction')
const notalogger = require('@appliedblockchain/not-a-logger')

describe('Fabric workflow', function() {
  this.timeout(100000)
  it('doit', async function() {
    const config = getConfig('alpha', 'Admin')
    const { client, channel, peers, queryPeer } = await makeClient(config)
    const result = await sendTransaction(notalogger, client, channel, 'simplechaincode', peers,
                          queryPeer, 'set', ['bar'])
  })
})
