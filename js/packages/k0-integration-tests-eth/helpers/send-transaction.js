'use strict'

const sendSignedTransaction = require('./send-signed-transaction')
const u = require('@appliedblockchain/k0-util')

async function sendTransaction(web3, to, data, gas = 50000000, privateKey) {
  if (to !== null) {
    console.log(to)
    u.checkBuf(to, 20)
  }
  u.checkBuf(data)
  u.checkBuf(privateKey, 32)
  const txParams = {
    to: to === null ? null : u.buf2hex(to),
    data: u.buf2hex(data),
    gas
  }
  const tx = await web3.eth.accounts.signTransaction(txParams, u.buf2hex(privateKey))
  return sendSignedTransaction(web3, u.hex2buf(tx.rawTransaction))
}

module.exports = sendTransaction
