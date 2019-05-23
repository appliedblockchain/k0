'use strict'

const _ = require('lodash')
const sendTransaction = require('../send-transaction')
const u = require('../../util')

async function transfer(logger, client, channel, chaincodeId, peers, queryPeer,
  in0sn, in1sn, out0cm, out1cm, out0data, out1data, newRoot) {
  const params = {
    in0sn: u.buf2hex(in0sn),
    in1sn: u.buf2hex(in1sn),
    out0cm: u.buf2hex(out0cm),
    out1cm: u.buf2hex(out1cm),
    out0data: u.buf2hex(out0data),
    out1data: u.buf2hex(out1data),
    newRoot: u.buf2hex(newRoot)
  }
  await sendTransaction(logger, client, channel, chaincodeId, peers, queryPeer,
                        'transfer', [ JSON.stringify(params) ])
}

module.exports = transfer
