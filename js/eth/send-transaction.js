const u = require('../util')

async function sendTransaction(web3, to, data, gas = 50000000, privateKey) {
  console.log('todatapriv', to.length, data.length, privateKey.length)
  console.log(to, data, privateKey)
  u.checkBuf(to, 20)
  u.checkBuf(data)
  u.checkBuf(privateKey, 32)
  const txParams = {
    to: u.buf2hex(to),
    data: u.buf2hex(data),
    gas
  }
  const tx = await web3.eth.accounts.signTransaction(txParams, u.buf2hex(privateKey))

  return new Promise((resolve, reject) => {
    const promiEvent = web3.eth.sendSignedTransaction(tx.rawTransaction)
    promiEvent.once('transactionHash', hex => resolve(u.hex2buf(hex)))
  })
}

module.exports = sendTransaction
