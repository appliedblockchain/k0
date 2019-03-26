const assert = require('assert')
const compileContracts = require('./test/helpers/compile-contracts')
const prepareDeposit = require('./actions/prepare-deposit')
const makeClient = require('./client')
const u = require('./util')

async function makeK0(a_sk, platform, state, serverPort = 4000) {
  u.checkBuf(a_sk, 32)
  console.log("Prover server port ", serverPort)
  const server = await makeClient(serverPort)
  const a_pk = await server.prf_addr(a_sk)
  const keys = {
    a_sk,
    a_pk
  }

  platform.on('deposit', async (txHash, cm, newRoot) => {
    u.checkBuf(txHash, 32)
    u.checkBuf(cm, 32)
    u.checkBuf(newRoot, 32)
    console.log('new roor', newRoot)
    await state.add(u.buf2hex(txHash), [cm], [], newRoot)
  })

  return {
    prepareDeposit: prepareDeposit.bind(null, server, keys)
  }
}

module.exports = makeK0
