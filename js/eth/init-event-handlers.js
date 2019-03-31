const handleDeposit = require('./event-handlers/deposit')
const handleTransfer = require('./event-handlers/transfer')
const u = require('../util')

function initEventHandlers(mvppt, eventEmitter) {
  let queue = []

  let processing = false
  let lastBlockNumber = 0
  let lastLogIndex = 0

  async function processQueue() {
    if (processing) {
      console.log('already processing. return.')
      return
    }
    processing = true
    var item = queue.shift()
    // await platformState.add(u.buf2hex(item.txHash), [item.cm], [], item.nextRoot)
    if (item.blockNumber < lastBlockNumber) {
      throw new Error('Received event out of order')
    } else if (item.blockNumber > lastBlockNumber) {
      lastBlockNumber = item.blockNumber
      lastLogIndex = 0
    }
    if (item.logIndex < lastLogIndex) {
      throw new Error('Received event out of order')
    }
    lastLogIndex = item.logIndex
    console.log('handling from queue', item.event, item.blockNumber, item.logIndex)
    if (item.event === 'Deposit') {
      await handleDeposit(eventEmitter, item)
    } else if (item.event === 'Transfer') {
      await handleTransfer(eventEmitter, item)
    } else if (item.event === 'Log') {
      console.log('Log', item.returnValues)
    } else if (item.event === 'SNRegistration') {
      console.log('SN Registration', item.returnValues.hash)
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
    } else {
      throw new Error(`Don't know what to do with event of type ${item.event}`)
    }
    await u.wait(200)
    processing = false
    if (queue.length > 0) {
      await processQueue()
    }
    console.log('going to sleep.')
  }


  mvppt.events.allEvents({ fromBlock: 0 }).on('data', event => {
    console.log('adding to queue', event.event, event.blockNumber, event.logIndex)
    queue.push(event)
    processQueue()
  })
}

module.exports = initEventHandlers
