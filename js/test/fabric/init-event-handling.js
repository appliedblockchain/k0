const handleMint = require('./event-handlers/mint')
const handleTransfer = require('./event-handlers/transfer')

function initEventHandling(platformState, secretStore, platform) {
  let queue = []

  let processing = false

  async function processQueue() {
    if (processing) {
      return
    }
    processing = true
    var item = queue.shift()

    if (item.type === 'mint'){
      await handleMint(platformState, item.txnid, ...item.params)
    } else if (item.type === 'transfer') {
      await handleTransfer(
        platformState,
        secretStore,
        item.txnid,
        ...item.params
      )
    } else {
      throw new Error(`Don't know what to do with event of type ${item.type}`)
    }

    // await u.wait(200)
    processing = false
    if (queue.length > 0) {
      processQueue()
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
    (txnid, snIn0, snIn1, cmOut0, cmOut1, dataOut0, dataOut1) => {
      addToQueue(
        'transfer',
        txnid,
        [
          snIn0,
          snIn1,
          cmOut0,
          cmOut1,
          dataOut0,
          dataOut1
        ]
      )
    }
  )
}

module.exports = initEventHandling
