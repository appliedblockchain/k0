const jayson = require('jayson/promise')
const cm = require('./cm')
const depositCommitmentProof = require('./deposit-commitment-proof')
const merkleTreeAdditionProof = require('./merkle-tree-addition-proof')
const prepareTransfer = require('./prepare-transfer')
const prepare_withdrawal = require('./prepare_withdrawal')
const prfAddr = require('./prf-addr')
const ready = require('./ready')
const verifyProof = require('./verify-proof')

function client(serverPort = 4000) {
  const jc = jayson.client.http({ port: serverPort })
  return {
    cm: cm.bind(null, jc),
    depositCommitmentProof: depositCommitmentProof.bind(null, jc),
    merkleTreeAdditionProof: merkleTreeAdditionProof.bind(null, jc),
    prepareTransfer: prepareTransfer.bind(null, jc),
    prepare_withdrawal: prepare_withdrawal.bind(null, jc),
    prfAddr: prfAddr.bind(null, jc),
    ready: ready.bind(null, jc),
    verifyProof: verifyProof.bind(null, jc)
  }
}

module.exports = client
