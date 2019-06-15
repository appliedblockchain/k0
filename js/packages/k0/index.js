'use strict'

const decryptNote = require('./actions/decrypt-note')
const generatePaymentData = require('./actions/generate-payment-data')
const makeClient = require('./client')
const pack256Bits = require('./actions/pack-256-bits')
const prepareDeposit = require('./actions/prepare-deposit')
const prepareTransfer = require('./actions/prepare-transfer')

async function makeK0(serverEndpoint = 'http://localhost:4000/') {
  const server = await makeClient(serverPort)
  return {
    decryptNote: decryptNote.bind(null, server),
    deriveKeys: server.deriveKeys,
    generatePaymentData: generatePaymentData.bind(null, server),
    pack256Bits: pack256Bits.bind(null, server),
    prepareDeposit: prepareDeposit.bind(null, server),
    prepareTransfer: prepareTransfer.bind(null, server)
  }
}

module.exports = makeK0
