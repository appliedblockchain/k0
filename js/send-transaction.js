async function sendTransaction(web3, to, data, gas = 50000000) {
  const txParams = {
    to,
    data,
    gas
  }
  const account = web3.eth.accounts.create()
  const tx = await account.signTransaction(txParams)
  return web3.eth.sendSignedTransaction(tx.rawTransaction)
}

module.exports = sendTransaction
