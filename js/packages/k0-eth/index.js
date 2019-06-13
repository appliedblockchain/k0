'use strict'

const EventEmitter = require('events')

// TODO Do something different here (include artefacts in repo?)
const compileContracts = require('./helpers/compile-contracts')
const deposit = require('./actions/deposit')
const merkleTreeRoot = require('./actions/merkle-tree-root')
const transfer = require('./actions/transfer')

const u = require('@appliedblockchain/k0-util')
const initEventHandlers = require('./init-event-handlers')

class K0Eth extends EventEmitter {}

// K0, the Ethereum parts. Encapsulates all the functionality needed to send
// transactions to Ethereum and listen to Ethereum events. The API should be
// abstract, so that in the future similar objects can be provided for other
// chains
async function makeEth(web3, mvpptAddress) {
  u.checkBuf(mvpptAddress, 20)
  const artefacts = await compileContracts()
  const mvppt = new web3.eth.Contract(
    artefacts.MVPPT.abi,
    u.buf2hex(mvpptAddress)
  )
  const k0Eth = new K0Eth()
  initEventHandlers(mvppt, k0Eth)
  k0Eth.deposit = deposit.bind(null, web3, mvppt)
  k0Eth.merkleTreeRoot = merkleTreeRoot.bind(null, web3, mvppt)
  k0Eth.transfer = transfer.bind(null, web3, mvppt)
  return k0Eth
}

module.exports = makeEth
