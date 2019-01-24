const sendTransaction = require('./send-transaction')

async function deploy(web3, abi, bytecode, gas, args) {
  const contract = new web3.eth.Contract(abi)
  const data = contract.deploy({
    data: bytecode,
    arguments: args
  }).encodeABI()
  const receipt = await sendTransaction(web3, null, data, gas)
  return receipt.contractAddress
}

module.exports = deploy