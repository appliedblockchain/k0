'use strict'

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

    if (item.type === 'Mint') {
      u.checkBuf(item.payload, 200)
      eventEmitter.emit(
        'mint',
        item.txnid,
        item.payload.slice(0, 32),
        item.payload.slice(32, 168),
        item.payload.slice(168)
      )
    } else if (item.type === 'Transfer') {
      u.checkBuf(item.payload, 432)
      eventEmitter.emit(
        'transfer',
        item.txnid,
        item.payload.slice(0,32),
        item.payload.slice(32,64),
        item.payload.slice(64,96),
        item.payload.slice(96,128),
        item.payload.slice(128,264),
        item.payload.slice(264,400),
        item.payload.slice(400)
      )
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
      queue.push({ type: event.event_name, txnid, payload: event.payload})
      processQueue()
    },
    error => console.log(error),
    {startBlock: 0}
  )

  return regId1
}

module.exports = initEventHandlers
