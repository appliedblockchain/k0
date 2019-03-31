const BN = require('bn.js')
const inquirer = require('inquirer')
const u = require('../util')
const ethUtil = require('../eth/util')
const deployContract = require('./deploy-contract')

async function deployTradingContract(web3, artefacts, secretStore,
                                     ethPrivateKey, k0, carToken,
                                     mvpptAddress) {
  u.checkBuf(ethPrivateKey, 32)
  u.checkBuf(mvpptAddress, 20)
  const carIdInquiryResult = await inquirer.prompt([
    { type: 'number', name: 'carId', message: 'Car ID' }
  ])
  const carId = new BN(carIdInquiryResult.carId)
  const priceInquiryResult = await inquirer.prompt([
    { type: 'number', name: 'price', message: 'Price' }
  ])
  const v = new BN(priceInquiryResult.price)
  const { rho, r, cm } = await k0.generatePaymentData(secretStore, v)
  console.log(`  rho: ${u.buf2hex(rho)}`)
  console.log(`  r:   ${u.buf2hex(r)}`)
  console.log(`  v:   ${v.toString()}`)
  console.log(`  cm:  ${u.buf2hex(cm)}`)

  const cmPacked = await ethUtil.pack256Bits(cm)

  const tradeContractAddress = await deployContract(
    web3,
    artefacts.HiddenPriceCarTrade,
    [
      carToken._address,
      u.buf2hex(mvpptAddress),
      carId.toString(),
      cmPacked[0].toString(),
      cmPacked[1].toString()
    ],
    ethPrivateKey
  )
  console.log(`  Contract address: ${u.buf2hex(tradeContractAddress)}`)

  const approvalTxData = carToken.methods.approve(
    u.buf2hex(tradeContractAddress), carId.toString()
  ).encodeABI()

  const txParams = {
    to: carToken._address,
    data: approvalTxData,
    gas: 1000000
  }
  const tx = await web3.eth.accounts.signTransaction(txParams, u.buf2hex(ethPrivateKey))
  const receipt = await web3.eth.sendSignedTransaction(tx.rawTransaction)
  assert(receipt.status === true)
}

module.exports = deployTradingContract
