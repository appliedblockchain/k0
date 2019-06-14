'use strict'

const conv = require('../../proof-conversion')
const sendTransaction = require('../send-transaction')
const u = require('../../util')

async function mint(logger, client, channel, chaincodeId, peers, queryPeer, k,
  v, cm, data, newRoot, commitmentProof, additionProof) {

  [ k, cm, newRoot ].forEach(buf => u.checkBuf(buf, 32))
  u.checkBuf(data)
  u.checkBN(v)
  const params = [
    k,
    v.toBuffer('be', 8),
    cm,
    data,
    newRoot,
    JSON.stringify(conv.stringifyJacobian(commitmentProof)),
    JSON.stringify(conv.stringifyJacobian(additionProof))
  ]

  const receipt = await sendTransaction(logger, client, channel, chaincodeId, peers, queryPeer,
    'mint', params)

  return receipt
}

module.exports = mint
