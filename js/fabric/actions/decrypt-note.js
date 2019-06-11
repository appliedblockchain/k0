'use strict'
const _ = require('lodash')
const conv = require('../../proof-conversion')
const sendTransaction = require('../send-transaction')
const u = require('../../util')

async function decryptNote(logger, client, channel, chaincodeId, peers,
                           queryPeer, k, v, cm, data, newRoot, commitmentProof,
                           additionProof) {
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
  await sendTransaction(logger, client, channel, chaincodeId, peers, queryPeer,
                        'mint', params)
}

module.exports = mint
