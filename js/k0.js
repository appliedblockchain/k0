const assert = require('assert')
const compileContracts = require('./test/helpers/compile-contracts')
const hexOfBuffer = require('@appliedblockchain/helpers/hex-of-buffer')
const hex0xOfHex = require('@appliedblockchain/helpers/hex0x-of-hex')
const deposit = require('actions/deposit')


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

async function makeK0(web3, mvpptAddress) {
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

module.exports = makeK0
