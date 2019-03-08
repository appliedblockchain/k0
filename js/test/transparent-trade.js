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

    dollarCoin = await util.deployStandardContract(
      web3,
      'DollarCoin',
      tokenMaster
    )
    carToken = await util.deployStandardContract(
      web3,
      'CarToken',
      carManufacturer
    )
  })


  it('Full cycle', async () => {
    const carId = "1"

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

    await printState(dollarCoin, carToken, accounts, accountNames, carIds)

    const askPrice = new BN("50000")

    write([
      `Alice: Deploying offer smart contract (offer to sell car ${carId} for `,
      `${askPrice})...`
    ].join(''))
    const tradeContract = await util.deployStandardContract(
      web3,
      'CarTrade',
      accounts.alice,
      [carToken._address, dollarCoin._address, carId, askPrice.toString()]
    )
    write('done.\n')

    write(`Alice: Allow smart contract to transfer CarToken ${carId}...`)
    await sendTransaction(
      web3,
      carToken._address,
      carToken.methods.approve(tradeContract._address, carId).encodeABI(),
      5000000,
      accounts.alice
    )
    write('done.\n')

    write(`Bob: Allow smart contract to withdraw ${askPrice} DollarCoin...`)
    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods.approve(tradeContract._address, askPrice.toString()).encodeABI(),
      5000000,
      accounts.bob
    )
    write('done.\n')

    const [aliceDollarsBefore, bobDollarsBefore] = await Promise.all(
      ['alice', 'bob'].map(async id => {
        return new BN(
          await dollarCoin.methods.balanceOf(accounts[id].address).call()
        )
      })
    )

    write('Bob: Call smart contract function "buy" (finalise the trade)...')
    await sendTransaction(
      web3,
      tradeContract._address,
      tradeContract.methods.buy().encodeABI(),
      5000000,
      accounts.bob
    )
    write('done.\n')

    const [aliceDollarsAfter, bobDollarsAfter] = await Promise.all(
      ['alice', 'bob'].map(async id => {
        return new BN(
          await dollarCoin.methods.balanceOf(accounts[id].address).call()
        )
      })
    )

    // Bob is the new owner of the car
    const newOwner = await carToken.methods.ownerOf(carId).call()
    expect(newOwner).to.equal(accounts.bob.address)
    // Alice's balance has increased by the ask price
    assert(aliceDollarsAfter.eq(aliceDollarsBefore.add(askPrice)))
    // Bob's balance has decreased by the ask price
    assert(bobDollarsAfter.eq(bobDollarsBefore.sub(askPrice)))


    await printState(dollarCoin, carToken, accounts, accountNames, carIds)
  })

})