'use strict'

async function waitForTx(channel, queryPeer, transactionId, promise) {
  const eh = channel.newChannelEventHub(queryPeer)
  eh.connect()
  const [ result, _ ] = await Promise.all([
    promise,
    new Promise((resolve, reject) => {
      eh.registerTxEvent(
        transactionId,
        (tx, code) => {
          eh.unregisterTxEvent(transactionId)
          eh.close()
          resolve()
        },
        (err) => {
          eh.unregisterTxEvent(transactionId)
          eh.close()
          reject(err)
        }
      )
    })
  ])
  return result
}

module.exports = waitForTx
