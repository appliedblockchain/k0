const assert = require('assert')
// Move this out of tests
const compileContracts = require('../test/helpers/compile-contracts')
const hex0xOfHex = require('@appliedblockchain/helpers/hex0x-of-hex')
const hexOfBuffer = require('@appliedblockchain/helpers/hex-of-buffer')
const deposit = require('./actions/deposit')

function initContractEventHandlers(mvppt) {
  mvppt.events.Deposit().on('data', event => {
    console.log('got Deposit event', event)
  })
  mvppt.events.Transfer().on('data', event => {
    console.log('got Transfer event', event)
  })
  mvppt.events.Withdrawal().on('data', event => {
    console.log('got Withdrawal event', event)
  })
}

// K0, the Ethereum parts. Encapsulates all the functionality needed to send
// transactions to Ethereum and listen to Ethereum events. The API should be
// abstract, so that in the future similar objects can be provided for other
// chains
async function makeEth(web3, mvpptAddress) {
  assert(Buffer.isBuffer(mvpptAddress) && mvpptAddress.length === 20,
         'MVPPT address is not a 20 byte buffer')
  const artefacts = await compileContracts()
  console.log(Object.keys(artefacts))
  console.log("address", mvpptAddress)
  const mvppt = new web3.eth.Contract(
    artefacts.MVPPT.abi,
    hex0xOfHex(hexOfBuffer(mvpptAddress))
  )
  initContractEventHandlers(mvppt)
  return {
    deposit: deposit.bind(null)
  }
}

module.exports = makeEth