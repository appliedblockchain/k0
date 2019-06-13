const sendTransaction = require('./send-transaction')
const u = require('@appliedblockchain/k0-util')

async function deploy(web3, abi, bytecode, gas, args, account = null) {
  if (account === null) {
    account = web3.eth.accounts.create()
  }
  const contract = new web3.eth.Contract(abi)
  const data = contract.deploy({
    data: bytecode,
    arguments: args
  }).encodeABI()
  const txHash = await sendTransaction(
    web3,
    null,
    u.hex2buf(data),
    gas,
    u.hex2buf(account.privateKey)
  )
  // TODO wait until tx is mined
  const receipt = await web3.eth.getTransactionReceipt(u.buf2hex(txHash))
  assert(receipt.contractAddress, 'No contract address in tx receipt')
  return u.hex2buf(receipt.contractAddress)
}

module.exports = deploy
