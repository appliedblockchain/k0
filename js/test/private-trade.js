const assert = require('assert')
const util = require('./util')
const sendTransaction = require('../send-transaction')
const log = console.log
const write = x => process.stdout.write(x)
const printState = require('./helpers/print-state')
const expect = require('code').expect
const BN = require('bn.js')

describe('Transparent trade', async () => {

  // the web3 object
  let web3

  // map accountIdentifier => web3 account
  let accounts

  // map address => account name
  let accountNames

  // list of car ids in the system (for display purposes)
  let carIds

  // DollarCoin minter account
  let tokenMaster

  // CarToken minter account
  let carManufacturer

  // contract object for ERC-20 token
  let dollarCoin

  // contract object for ERC-721 token
  let carToken

  before(async () => {
    web3 = util.initWeb3()
    accounts = {}
    accountNames = {}
    carIds = []
    const names = [
      ['alice', 'Alice'],
      ['bob', 'Bob']
    ].forEach(pair => {
      const account = web3.eth.accounts.create()
      accounts[pair[0]] = account
      accountNames[account.address] = pair[1]
    })

    tokenMaster = web3.eth.accounts.create()
    carManufacturer = web3.eth.accounts.create()
    const artefacts = await compileContracts()
    dollarCoin = await util.deployContract(web3, artefacts.DollarCoin, [], tokenMaster)
    carToken = await util.deployContract(web3, artefacts.CarToken, [], carManufacturer)
  })

  it('Full cycle', async () => {
    log('hi')
  })
})