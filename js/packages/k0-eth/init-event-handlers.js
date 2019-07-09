const _ = require('lodash')
const handleDeposit = require('./event-handlers/deposit')
const handleTransfer = require('./event-handlers/transfer')
const u = require('@appliedblockchain/k0-util')

function initEventHandlers(mvppt, server, eventEmitter) {
  const queue = []

  let processing = false
  let lastBlockNumber = 0
  let lastTransactionIndex = 0

  async function processQueue() {
    if (processing) {
      return
    }
    processing = true
    var item = queue.shift()
    if (item.blockNumber < lastBlockNumber) {
      console.log(item)
      throw new Error(`Received event out of order (blockNumber: ${item.blockNumber}, last: ${lastBlockNumber})`)
    } else if (item.blockNumber > lastBlockNumber) {
      lastTransactionIndex = 0
    }
    if (item.transactionIndex < lastTransactionIndex) {
      console.log(item)
      throw new Error(`Received event out of order (transactionIndex: ${item.transactionIndex}, last: ${lastTransactionIndex})`)
    }

    if (item.event === 'Deposit') {
      await handleDeposit(server, eventEmitter, item)
    } else if (item.event === 'Transfer') {
      await handleTransfer(server, eventEmitter, item)
    } else if (item.event === 'SNRegistration') {
      // ignore
    } else if (item.event === 'PublicInputsCommitment') {
      // ignore
    } else if (item.event === 'PublicInputsAddition') {
      // ignore
    } else if (item.event === 'PublicInputsTransfer') {
      // ignore
    } else {
      console.log(`Don't know what to do with event of type ${item.event}`)
      console.log(item)
    }
    await u.wait(200)
    lastBlockNumber = item.blockNumber
    lastTransactionIndex = item.transactionIndex
    processing = false
    if (queue.length > 0) {
      await processQueue()
    }
  }


  mvppt.events.allEvents({ fromBlock: 0 }).on('data', event => {
    queue.push(event)
    processQueue()
  })
}

module.exports = initEventHandlers
