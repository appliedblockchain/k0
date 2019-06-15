'use strict'

const sendTransaction = require('../send-transaction')

async function transfer(logger, client, channel, chaincodeId, peers, queryPeer,
  in0sn, in1sn, out0cm, out1cm, out0data, out1data, newRoot) {
  const params = [ in0sn, in1sn, out0cm, out1cm, out0data, out1data, newRoot ]
  await sendTransaction(logger, client, channel, chaincodeId, peers, queryPeer,
    'transfer', params)
}

module.exports = transfer
