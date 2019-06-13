const signTransaction = require('../eth/sign-transaction')
const u = require('@appliedblockchain/k0-util')

async function deployContract(web3, artefacts, params, privateKey, gas = 1000000) {
  u.checkBuf(privateKey, 32)
  const contract = new web3.eth.Contract(artefacts.abi)
  const data = contract.deploy({
    data: artefacts.bytecode,
    arguments: params
  }).encodeABI()
  const txParams = { data, gas }
  const tx = await web3.eth.accounts.signTransaction(txParams, u.buf2hex(privateKey))
  const receipt = await web3.eth.sendSignedTransaction(tx.rawTransaction)
  return u.hex2buf(receipt.contractAddress)
}

module.exports = deployContract
