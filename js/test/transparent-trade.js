const _ = require('lodash')
const assert = require('assert')
const compileContracts = require('./helpers/compile-contracts')
const util = require('./util')
const sendTransaction = require('../send-transaction')
const log = console.log
const write = x => process.stdout.write(x)
const printState = require('./helpers/print-state')
const expect = require('code').expect
const BN = require('bn.js')
const commonTradingInit = require('./helpers/common-trading-init')

describe('Transparent trade', async () => {
  let web3, accounts, accountAddresses, accountNames, carIds, tokenMaster, carManufacturer,
    artefacts, dollarCoin, carToken, carId

  before(async () => {
    const initResult = await commonTradingInit()
    web3 = initResult.web3
    accounts = initResult.accounts
    accountNames = initResult.accountNames
    carIds = initResult.carIds
    tokenMaster = initResult.tokenMaster
    carManufacturer = initResult.carManufacturer
    artefacts = initResult.artefacts
    dollarCoin = initResult.dollarCoin
    carToken = initResult.carToken
    carId = initResult.carId
    accountAddresses = _.map(accounts, 'address')
  })

  it('Full cycle', async function() {

    this.timeout(3600 * 1000)

    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)

    const askPrice = new BN("50000")

    await util.prompt()

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

    await util.prompt()

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


    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)
  })

})