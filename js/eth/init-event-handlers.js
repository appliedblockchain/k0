const handleDeposit = require('./event-handlers/deposit')
const handleTransfer = require('./event-handlers/transfer')

function initEventHandlers(mvppt, eventEmitter) {
  let queue = []

  let processing = false
  let lastBlockNumber = 0
  let lastLogIndex = 0

  async function processQueue() {
    if (processing) {
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
    console.log("PROCESSING", lastBlockNumber, lastLogIndex, item.event)
    if (item.event === 'Deposit') {
      await handleDeposit(eventEmitter, item)
    } else if (item.event === 'Transfer') {
      await handleTransfer(eventEmitter, item)
    } else if (item.event === 'Log') {
      // ignore
    } else {
      throw new Error(`Don't know what to do with event of type ${item.event}`)
    }
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
