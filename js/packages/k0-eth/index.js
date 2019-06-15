'use strict'

const EventEmitter = require('events')

const deposit = require('./actions/deposit')
const merkleTreeRoot = require('./actions/merkle-tree-root')
const transfer = require('./actions/transfer')

const u = require('@appliedblockchain/k0-util')
const initEventHandlers = require('./init-event-handlers')
const mvpptAbi = require('./mvppt.abi')

class K0Eth extends EventEmitter {}

// K0, the Ethereum parts. Encapsulates all the functionality needed to send
// transactions to Ethereum and listen to Ethereum events.
async function makeEth(web3, mvpptAddress) {
  u.checkBuf(mvpptAddress, 20)
  const mvppt = new web3.eth.Contract(
    mvpptAbi,
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
