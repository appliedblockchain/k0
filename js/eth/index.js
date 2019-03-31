const EventEmitter = require('events')
const assert = require('assert')
// Move this out of tests
const compileContracts = require('../test/helpers/compile-contracts')
const hex0xOfHex = require('@appliedblockchain/helpers/hex0x-of-hex')
const hexOfBuffer = require('@appliedblockchain/helpers/hex-of-buffer')
const deposit = require('./actions/deposit')
const merkleTreeRoot = require('./actions/merkle-tree-root')
const transfer = require('./actions/transfer')
const ethUtil = require('./util')
const BN = require('bn.js')
const u = require('../util')
const initEventHandlers = require('./init-event-handlers')

class K0Eth extends EventEmitter {}

// K0, the Ethereum parts. Encapsulates all the functionality needed to send
// transactions to Ethereum and listen to Ethereum events. The API should be
// abstract, so that in the future similar objects can be provided for other
// chains
async function makeEth(web3, mvpptAddress) {
  assert(Buffer.isBuffer(mvpptAddress) && mvpptAddress.length === 20,
         'MVPPT address is not a 20 byte buffer')
  const artefacts = await compileContracts()
  const mvppt = new web3.eth.Contract(
    artefacts.MVPPT.abi,
    hex0xOfHex(hexOfBuffer(mvpptAddress))
  )
  const k0Eth = new K0Eth()
  initEventHandlers(mvppt, k0Eth)
  k0Eth.deposit = deposit.bind(null, web3, mvppt)
  k0Eth.merkleTreeRoot = merkleTreeRoot.bind(null, web3, mvppt)
  k0Eth.transfer = transfer.bind(null, web3, mvppt)
  return k0Eth
}

module.exports = makeEth
