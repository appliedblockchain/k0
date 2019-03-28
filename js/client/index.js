const jayson = require('jayson/promise')
const cm = require('./cm')
const depositCommitmentProof = require('./deposit-commitment-proof')
const merkleTreeAdditionProof = require('./merkle-tree-addition-proof')
const prepare_transfer = require('./prepare_transfer')
const prepare_withdrawal = require('./prepare_withdrawal')
const prfAddr = require('./prf-addr')
const ready = require('./ready')

function client(serverPort = 4000) {
  const jc = jayson.client.http({ port: serverPort })
  return {
    cm: cm.bind(null, jc),
    depositCommitmentProof: depositCommitmentProof.bind(null, jc),
    merkleTreeAdditionProof: merkleTreeAdditionProof.bind(null, jc),
    prepare_transfer: prepare_transfer.bind(null, jc),
    prepare_withdrawal: prepare_withdrawal.bind(null, jc),
    prfAddr: prfAddr.bind(null, jc),
    ready: ready.bind(null, jc)
  }
}

module.exports = client
