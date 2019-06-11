'use strict'

const generatePaymentData = require('./actions/generate-payment-data')
const makeClient = require('./client')
const prepareDeposit = require('./actions/prepare-deposit')
const prepareTransfer = require('./actions/prepare-transfer')

async function makeK0(serverPort = 4000) {
  const server = await makeClient(serverPort)
  return {
    generatePaymentData: generatePaymentData.bind(null, server),
    prepareDeposit: prepareDeposit.bind(null, server),
    prepareTransfer: prepareTransfer.bind(null, server)
  }
}

module.exports = makeK0
