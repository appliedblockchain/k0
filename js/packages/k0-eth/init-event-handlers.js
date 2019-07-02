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
      console.log('Commitment public inputs')
      _.range(5).forEach(idx => {
        const num = item.returnValues[idx.toString()]
        console.log(num.toString(16))
      })
      console.log()
    } else if (item.event === 'PublicInputsAddition') {
      console.log('Addition public inputs')
      _.range(7).forEach(idx => {
        const num = item.returnValues[idx.toString()]
        console.log(num.toString(16))
      })
      console.log()
    } else if (item.event === 'PublicInputsTransfer') {
      console.log('Transfer public inputs')
      _.range(11).forEach(idx => {
        const num = item.returnValues[idx.toString()]
        console.log(num.toString(16))
      })
      console.log()
    } else {
      throw new Error(`Don't know what to do with event of type ${item.event}`)
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
