const EventEmitter = require('events')
const handleMint = require('./event-handlers/mint')
const handleTransfer = require('./event-handlers/transfer')
const u = require('../../util')

class EventHub extends EventEmitter {}

function makeEventHub(platformState, secretStore, platform) {
  let queue = []

  let processing = false

  const eh = new EventHub()

  async function processQueue() {
    if (processing || queue.length === 0) {
      return
    }
    processing = true
    var item = queue.shift()

    if (item.type === 'mint'){
      await handleMint(platformState, item.txnid, ...item.params)
      eh.emit('mintProcessed', item.txnid)
    } else if (item.type === 'transfer') {
      await handleTransfer(
        platformState,
        secretStore,
        item.txnid,
        ...item.params
      )
      eh.emit('transferProcessed', item.txnid)
    } else {
      throw new Error(`Don't know what to do with event of type ${item.type}`)
    }

    processing = false
    if (queue.length === 0) {
      eh.emit('queueEmpty')
    } else {
      await processQueue()
    }
  }

  function addToQueue(type, txnid, params) {
    queue.push({ type, txnid, params })
    processQueue()
  }

  platform.on('mint', (txnid, cm, nextRoot) => {
    addToQueue('mint', txnid, [cm, nextRoot])
  })

  platform.on(
    'transfer',
    (txnid, snIn0, snIn1, cmOut0, cmOut1, dataOut0, dataOut1, root) => {
      addToQueue(
        'transfer',
        txnid,
        [
          snIn0,
          snIn1,
          cmOut0,
          cmOut1,
          dataOut0,
          dataOut1,
          root
        ]
      )
    }
  )

  eh.queueEmpty = () => queue.length === 0

  return eh
}

module.exports = makeEventHub
