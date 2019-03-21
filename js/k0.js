const assert = require('assert')
const compileContracts = require('./test/helpers/compile-contracts')
const prepareDeposit = require('./actions/prepare-deposit')
const makeClient = require('./client')

async function makeK0(a_sk, serverPort = 4000) {
  assert(Buffer.isBuffer(a_sk) && a_sk.length === 32)
  const server = await makeClient(serverPort)
  const a_pk = await server.prf_addr(a_sk)
  const keys = {
    a_sk,
    a_pk 
  }
  return {
    prepareDeposit: prepareDeposit.bind(null, server, keys)
  }
}

module.exports = makeK0
