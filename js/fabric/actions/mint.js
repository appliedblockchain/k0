'use strict'
const _ = require('lodash')
const conv = require('../../proof-conversion')
const sendTransaction = require('../send-transaction')
const u = require('../../util')

async function mint(logger, client, channel, chaincodeId, peers, queryPeer, k,
                    v, cm, data, newRoot, commitmentProof, additionProof) {
  [k,cm,newRoot].forEach(buf => u.checkBuf(k, 32))
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
