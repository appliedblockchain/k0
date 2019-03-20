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

describe('Transparent trade', async function() {
  this.timeout(100 * 1000)
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
    const tradeContract = await util.deployStandardContract(
      web3,
      'CarTrade',
      accounts.alice,
      [carToken._address, dollarCoin._address, carId, askPrice.toString()]
    )
    log(`Transaction from ${accounts.alice.address} (Alice):`)
    log(`- Deployment of a trading smart contract ${tradeContract._address} (offer to sell car ${carId} for ${askPrice})`)

    await sendTransaction(
      web3,
      carToken._address,
      carToken.methods.approve(tradeContract._address, carId).encodeABI(),
      5000000,
      accounts.alice
    )

    await util.prompt()

    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods.approve(tradeContract._address, askPrice.toString()).encodeABI(),
      5000000,
      accounts.bob
    )

    const [aliceDollarsBefore, bobDollarsBefore] = await Promise.all(
      ['alice', 'bob'].map(async id => {
        return new BN(
          await dollarCoin.methods.balanceOf(accounts[id].address).call()
        )
      })
    )

    await sendTransaction(
      web3,
      tradeContract._address,
      tradeContract.methods.buy().encodeABI(),
      5000000,
      accounts.bob
    )
    log(`Transaction from ${accounts.bob.address} (Bob):`)
    log(`- Calling of smart contract function "buy" on ${tradeContract._address}`)
    log(`- Transfer of ${askPrice} DollarCoin from Bob to Alice`)
    log(`- Transfer of CarToken ${carId} from Alice to Bob`)

    await util.prompt()

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
