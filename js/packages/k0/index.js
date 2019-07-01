'use strict'

const decryptNote = require('./actions/decrypt-note')
const generatePaymentData = require('./actions/generate-payment-data')
const makeClient = require('@appliedblockchain/k0-server-client')
const pack256Bits = require('./actions/pack-256-bits')
const prepareDeposit = require('./actions/prepare-deposit')
const prepareTransfer = require('./actions/prepare-transfer')
const ready = require('./actions/ready')
const unpack256Bits = require('./actions/unpack-256-bits')

async function makeK0(serverEndpoint) {
  if (!serverEndpoint) {
    throw new Error('No server endpoint provided for k0')
  }
  const server = await makeClient(serverEndpoint)
  return {
    decryptNote: decryptNote.bind(null, server),
    deriveKeys: server.deriveKeys,
    generatePaymentData: generatePaymentData.bind(null, server),
    pack256Bits: pack256Bits.bind(null, server),
    prepareDeposit: prepareDeposit.bind(null, server),
    prepareTransfer: prepareTransfer.bind(null, server),
    unpack256Bits: unpack256Bits.bind(null, server),
    ready: ready.bind(null, server)
  }
}

module.exports = makeK0
