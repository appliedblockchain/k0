const assert = require('assert')
const compileContracts = require('./test/helpers/compile-contracts')
const prepareDeposit = require('./actions/prepare-deposit')
const prepareTransfer = require('./actions/prepare-transfer')
const makeClient = require('./client')
const u = require('./util')

async function makeK0(serverPort = 4000) {
  const server = await makeClient(serverPort)
  return {
    prepareDeposit: prepareDeposit.bind(null, server),
    prepareTransfer: prepareTransfer.bind(null, server),
    prfAddr: server.prfAddr
  }
}

module.exports = makeK0
