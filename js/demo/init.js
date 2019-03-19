'use strict'

const compileContracts = require('../test/helpers/compile-contracts')
const fs = require('fs')
const testUtil = require('../test/util')
const client = require('../client')

async function run() {

  const web3 = testUtil.initWeb3()
  // DollarCoin minter
  const tokenMaster = web3.eth.accounts.create()

  // CarToken minter
  const carManufacturer = web3.eth.accounts.create()

  // contract artefacts
  const artefacts = await compileContracts()

  // ERC-20 payment token
  const dollarCoin = await testUtil.deployContract(
    web3, artefacts.DollarCoin, [], tokenMaster)

  // ERC-721 token representing cars
  const carToken = await testUtil.deployContract(
    web3, artefacts.CarToken, [], carManufacturer)

  const verifierAddresses = await Promise.all([
    'CommitmentVerifier',
    'AdditionVerifier',
    'TransferVerifier',
    'WithdrawalVerifier'
  ].map(async name => {
    const contract = await testUtil.deployContract(web3, artefacts[name])
    return contract._address
  }))

  const server = client()
  const initialRoot = await server.root()
  const mvppt = await testUtil.deployContract(
    web3,
    artefacts.MVPPT,
    [
      dollarCoin._address,
      ...verifierAddresses,
      await testUtil.pack256Bits(initialRoot)
    ]
  )

  fs.writeFileSync('artefacts.json', JSON.stringify(artefacts))
  fs.writeFileSync('addresses.json', JSON.stringify({
    DollarCoin: dollarCoin._address,
    CarToken: carToken._address,
    MVPPT: mvppt._address
  }))
}

run().then(console.log).catch(console.log)
