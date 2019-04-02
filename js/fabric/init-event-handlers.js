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
    console.log('item', item)

    if (item.type === 'Mint'){
      console.log({ payload: item.payload })
      eventEmitter.emit('mint', item.txnid, item.payload[0], item.payload[1])
   // if (item.event === 'Deposit') {
   //    await handleDeposit(eventEmitter, item)
   //  } else if (item.event === 'Transfer') {
   //    await handleTransfer(eventEmitter, item)
   //  } else if (item.event === 'Log') {
   //    // ignore
   //  } else if (item.event === 'SNRegistration') {
   //    // ignore
   //  } else if (item.event === 'SNReuseAttemptError') {
   //    console.log('SN reuse attempt', item.returnValues.pos, item.returnValues.hash)
   //    process.exit(1)
   //  } else if (item.event === 'DepositFailure') {
   //    console.log('DepositFailure')
   //    process.exit(1)
   //  } else if (item.event === 'TransferFailure') {
   //    console.log('TransferFailure')
   //    process.exit(1)
   //  } else if (item.event === 'TransferFromFailure') {
   //    console.log('TransferFromFailure')
   //    process.exit(1)
    } else {
      throw new Error(`Don't know what to do with event of type ${item.event}`)
     }

    await u.wait(200)
    // lastBlockNumber = item.blockNumber
    // lastTransactionIndex = item.transactionIndex
    processing = false
    if (queue.length > 0) {
      await processQueue()
    }
  }

  const regId1 = channelEventHub.registerChaincodeEvent(
    chaincodeId,
    '^.*',
    (event, block_num, txnid, status) => {
      console.log(txnid)
      let event_payload = JSON.parse(event.payload)
      console.log('adding to queue')
      queue.push({ type: event.event_name, txnid, payload: event_payload})
      processQueue()
    },
    console.log,
    {startBlock: 0})

  return regId1
      // if (event.event_name === 'Mint') {
      //   fabric.emit('mint', txnid, ...event_payload)
      //   console.log('emitted')
      //   console.log('Mint', event_payload)
      // } else {
      //   console.log('unknown event.')
      // }


}

module.exports = initEventHandlers
