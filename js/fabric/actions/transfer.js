'use strict'
const _ = require('lodash')
const sendTransaction = require('./send-transaction')

async function transfer(client, channel, chaincodeId, peers, in0sn, in1sn,
                        out0cm, out1cm, out0data, out1data, newRoot) {
	const params = [ in0sn, in1sn, out0cm, out1cm, out0data, out1data, newRoot]
  await sendTransaction(client, channel, chaincodeId, peers, 'transfer', params)
}

module.exports = transfer
