const u = require('../util')

async function sendTransaction(web3, to, data, gas = 50000000, account = null) {
    const txParams = {
      to,
      data,
      gas
    }
    if (account === null) {
      account = web3.eth.accounts.create()
    }
    const tx = await account.signTransaction(txParams)

  return new Promise((resolve, reject) => {
    const promiEvent = web3.eth.sendSignedTransaction(tx.rawTransaction)
    promiEvent.once('transactionHash', hex => resolve(u.hex2buf(hex)))
  })
}

module.exports = sendTransaction