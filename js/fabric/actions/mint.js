'use strict'
const _ = require('lodash')
const sendTransaction = require('./send-transaction')

async function mint(client, channel, chaincodeId, peers, cm, newRoot) {
	const params = [ cm, newRoot ]
  await sendTransaction(client, channel, chaincodeId, peers, 'mint', params)
}

module.exports = mint
