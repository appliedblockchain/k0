'use strict'

const deposit = require('./actions/deposit')
const EventEmitter = require('events')
const initEventHandlers = require('./init-event-handlers')
const makeClient = require('@appliedblockchain/k0-server-client')
const merkleTreeRoot = require('./actions/merkle-tree-root')
const mvpptAbi = require('./mvppt.abi')
const transfer = require('./actions/transfer')
const u = require('@appliedblockchain/k0-util')

class K0Eth extends EventEmitter {}

// K0, the Ethereum parts. Encapsulates all the functionality needed to send
// transactions to Ethereum and listen to Ethereum events.
// TODO Remove dependency on server(client). This is just for packing/unpacking
// which could be done in JS directly.
async function makeEth(web3, mvpptAddress, serverEndpoint) {
  if (!serverEndpoint) {
    throw new Error('No server endpoint specified for k0-eth')
  }
  u.checkBuf(mvpptAddress, 20)
  const server = await makeClient(serverEndpoint)
  const mvppt = new web3.eth.Contract(
    mvpptAbi,
    u.buf2hex(mvpptAddress)
  )
  const k0Eth = new K0Eth()
  initEventHandlers(mvppt, server, k0Eth)
  k0Eth.deposit = deposit.bind(null, web3, mvppt, server)
  k0Eth.merkleTreeRoot = merkleTreeRoot.bind(null, web3, mvppt, server)
  k0Eth.transfer = transfer.bind(null, web3, mvppt, server)
  return k0Eth
}

module.exports = makeEth
