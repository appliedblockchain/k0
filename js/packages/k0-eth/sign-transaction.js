const u = require('@appliedblockchain/k0-util')

async function signTransaction(web3, to, data, gas = 50000000, privateKey) {
  u.checkBuf(to, 20)
  u.checkBuf(data)
  u.checkBuf(privateKey, 32)
  const txParams = {
    to: u.buf2hex(to),
    data: u.buf2hex(data),
    gas,
    gasPrice: '0'
  }

  const tx = await web3.eth.accounts.signTransaction(
    txParams,
    u.buf2hex(privateKey)
  )
  return u.hex2buf(tx.rawTransaction)
}

module.exports = signTransaction
