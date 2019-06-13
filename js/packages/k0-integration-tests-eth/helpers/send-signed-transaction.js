const u = require('@appliedblockchain/k0-util')

function sendSignedTransaction(web3, tx) {
  u.checkBuf(tx)
  return new Promise((resolve, reject) => {
    const promiEvent = web3.eth.sendSignedTransaction(u.buf2hex(tx))
    promiEvent.once('transactionHash', hex => resolve(u.hex2buf(hex)))
  })
}

module.exports = sendSignedTransaction
