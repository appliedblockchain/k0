'use strict'

async function waitForTx(eventHub, transactionId, promise) {
  return new Promise((resolve, reject) => {
    eventHub.registerTxEvent(
      transactionId,
      (tx, code) => {
        eventHub.unregisterTxEvent(transactionId);
        resolve()
      },
      (err) => {
        eh.unregisterTxEvent(transactionId);
        reject(err)
      }
    )
  })
}

module.exports = waitForTx
