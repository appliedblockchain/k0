'use strict'
const _ = require('lodash')
const sendTransaction = require('../send-transaction')

async function mint(logger, client, channel, chaincodeId, peers, queryPeer, cm,
                    newRoot) {
	const params = [ cm, newRoot ]
  await sendTransaction(logger, client, channel, chaincodeId, peers, queryPeer,
                        'mint', params)
}

module.exports = mint
