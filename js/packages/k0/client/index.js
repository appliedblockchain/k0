const jayson = require('jayson/promise')
const cm = require('./cm')
const decryptNote = require('./decrypt-note')
const deriveKeys = require('./derive-keys')
const depositCommitmentProof = require('./deposit-commitment-proof')
const encryptNote = require('./encrypt-note')
const merkleTreeAdditionProof = require('./merkle-tree-addition-proof')
const pack256Bits = require('./pack-256-bits')
const prepareTransfer = require('./prepare-transfer')
const prepare_withdrawal = require('./prepare_withdrawal')
const prfAddr = require('./prf-addr')
const ready = require('./ready')
const verifyProof = require('./verify-proof')

function client(endpoint = 'http://localhost:4000/') {
  const jc = jayson.client.http(endpoint)
  return {
    cm: cm.bind(null, jc),
    decryptNote: decryptNote.bind(null, jc),
    depositCommitmentProof: depositCommitmentProof.bind(null, jc),
    deriveKeys: deriveKeys.bind(null, jc),
    encryptNote: encryptNote.bind(null, jc),
    merkleTreeAdditionProof: merkleTreeAdditionProof.bind(null, jc),
    pack256Bits: pack256Bits.bind(null, jc),
    prepareTransfer: prepareTransfer.bind(null, jc),
    prepare_withdrawal: prepare_withdrawal.bind(null, jc),
    prfAddr: prfAddr.bind(null, jc),
    ready: ready.bind(null, jc),
    verifyProof: verifyProof.bind(null, jc)
  }
}

module.exports = client
