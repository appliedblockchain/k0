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
    } else if (item.event === 'Log') {
      // console.log('LOG', require('util').inspect(item, { depth: null, colors: true }))
    } else if (item.event === 'SNRegistration') {
      // ignore
    } else if (item.event === 'SNReuseAttemptError') {
      console.log('SN reuse attempt', item.returnValues.pos, item.returnValues.hash)
      process.exit(1)
    } else if (item.event === 'DepositFailure') {
      console.log('DepositFailure')
      process.exit(1)
    } else if (item.event === 'TransferFailure') {
      console.log('TransferFailure')
      process.exit(1)
    } else if (item.event === 'TransferFromFailure') {
      console.log('TransferFromFailure')
      process.exit(1)
    } else if (item.event === 'Debug') {
      // console.log('ETH DEBUG', require('util').inspect(item, { depth: null, colors: true }))
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
