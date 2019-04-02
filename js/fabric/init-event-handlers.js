const u = require('../util')

function initEventHandlers(channelEventHub, chaincodeId, eventEmitter) {
  let queue = []

  let processing = false
  let lastBlockNumber = 0
  let lastTransactionIndex = 0

  async function processQueue() {
    if (processing) {
      return
    }
    processing = true
    var item = queue.shift()

    if (item.type === 'Mint'){
      eventEmitter.emit('mint', item.txnid, ...item.payload)
    } else if (item.type === 'Transfer') {
      eventEmitter.emit('transfer', item.txnid, ...item.payload)
    } else {
      throw new Error(`Don't know what to do with event of type ${item.type}`)
    }

    await u.wait(200)
    processing = false
    if (queue.length > 0) {
      await processQueue()
    }
  }

  const regId1 = channelEventHub.registerChaincodeEvent(
    chaincodeId,
    '^.*',
    (event, block_num, txnid, status) => {
      let event_payload = JSON.parse(event.payload)
      queue.push({ type: event.event_name, txnid, payload: event_payload})
      processQueue()
    },
    error => console.log(error),
    {startBlock: 0}
  )

  return regId1
}

module.exports = initEventHandlers
