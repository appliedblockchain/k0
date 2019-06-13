const util = require('@appliedblockchain/k0-util')
const compileContracts = require('./compile-contracts')
const log = console.log
const write = x => process.stdout.write(x)
const sendTransaction = require('../../send-transaction')
const printState = require('./print-state')
const BN = require('bn.js')
const chalk = require('chalk')

async function commonTradingInit(fancy = false) {
  const web3 = util.initWeb3()

  // map accountIdentifier => web3 account
  const accounts = {}

  // map address => account name
  const accountNames = {}

  // list of car ids in the system (for display purposes)
  const carIds = []

  const names = [
    ['alice', 'Alice'],
    ['bob', 'Bob']
  ].forEach(pair => {
    const account = web3.eth.accounts.create()
    accounts[pair[0]] = account
    accountNames[account.address] = pair[1]
  })

  // DollarCoin minter
  tokenMaster = web3.eth.accounts.create()

  // CarToken minter
  carManufacturer = web3.eth.accounts.create()

  // contract artefacts
  const artefacts = await compileContracts()

  // ERC-20 payment token
  const dollarCoin = await util.deployContract(
    web3, artefacts.DollarCoin, [], tokenMaster)

  // ERC-721 token representing cars
  carToken = await util.deployContract(
    web3, artefacts.CarToken, [], carManufacturer)
  const carId = "1"

  util.clear()

  if (fancy) {
    console.log(chalk.green([
      '',
      '██╗  ██╗ ██████╗',
      '██║ ██╔╝██╔═████╗',
      '█████╔╝ ██║██╔██║',
      '██╔═██╗ ████╔╝██║',
      '██║  ██╗╚██████╔╝',
      '╚═╝  ╚═╝ ╚═════╝',
      ''
    ].join('\n')))
  }

  write('Making a car for Alice...')
  await sendTransaction(
    web3,
    carToken._address,
    carToken.methods.mint(accounts.alice.address, carId).encodeABI(),
    5000000,
    carManufacturer
  )
  carIds.push(carId)
  write('done.\n')

  write('Giving Bob some money...')
  const moneyCreationData = await sendTransaction(
    web3,
    dollarCoin._address,
    dollarCoin.methods.mint(accounts.bob.address, "100000").encodeABI(),
    5000000,
    tokenMaster
  )
  write('done.\n')

  return {
    web3,
    accounts,
    accountNames,
    carId,
    carIds,
    tokenMaster,
    carManufacturer,
    artefacts,
    dollarCoin,
    carToken
  }
}

module.exports = commonTradingInit