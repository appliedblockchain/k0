'use strict'

const _ = require('lodash')
const assert = require('assert')
const crypto = require('crypto')
const util = require('./util')
const sendTransaction = require('../send-transaction')
const log = console.log
const write = x => process.stdout.write(x)
const printState = require('./helpers/print-state')
const expect = require('code').expect
const BN = require('bn.js')
const compileContracts = require('./helpers/compile-contracts')
const commonTradingInit = require('./helpers/common-trading-init')
const jayson = require('jayson/promise')
const makeClient = require('../client')
const flattenProof = require('../flatten-proof')

describe('Private trade', async function () {

  this.timeout(20000)

  let web3, accounts, accountAddresses, accountNames, carIds, tokenMaster,
    carManufacturer, artefacts, dollarCoin, carToken, carId, mvppt, server

  before(async () => {

    server = makeClient()
    await server.ready()

    const initResult = await commonTradingInit(true)
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

    await server.reset()
    const initialRoot = await server.root()
    const verifierAddresses = await Promise.all([
      'CommitmentVerifier',
      'AdditionVerifier',
      'TransferVerifier',
      'WithdrawalVerifier'
    ].map(async name => {
      const contract = await util.deployContract(web3, artefacts[name])
      return contract._address
    }))
    mvppt = await util.deployContract(
      web3,
      artefacts.MVPPT,
      [
        dollarCoin._address,
        ...verifierAddresses,
        await util.pack256Bits(initialRoot)
      ]
    )
    accountAddresses.push(mvppt._address)
    accountNames[mvppt._address] = "K0Cash"
  })

  function sampleCoin(a_sk, v) {
    assert(BN.isBN(v))
    return {
      rho: util.randomBytes(32),
      r: util.randomBytes(48),
      a_sk,
      v
    }
  }

  it('Full cycle', async function () {
    this.timeout(3600 * 1000)

     // bob: Pay in 2x
    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)

    await util.prompt()

    const [a_sk_alice, a_sk_bob] = _.times(2, () => util.randomBytes(32))
    const [a_pk_alice, a_pk_bob] = await Promise.all(
      [a_sk_alice, a_sk_bob].map(a_sk => server.prf_addr(a_sk))
    )

    write("Bob: Deposit 70000...")
    const coins = [
      sampleCoin(a_sk_bob, new BN("30000")),
      sampleCoin(a_sk_bob, new BN("40000"))
    ]
    for (let i = 0; i < 2; i++) {

      const c = coins[i]
      await sendTransaction(
        web3,
        dollarCoin._address,
        dollarCoin.methods.approve(mvppt._address, c.v.toString()).encodeABI(),
        5000000,
        accounts.bob
      )

      const data = await server.prepare_deposit(a_pk_bob, c.rho, c.r, c.v.toString())
      const commitmentProofCompact = flattenProof(data.commitmentProof)
      const additionProofCompact = flattenProof(data.additionProof)

      const params = [
        c.v.toString(),
        await util.pack256Bits(data.k),
        await util.pack256Bits(data.cm),
        await util.pack256Bits(data.nextRoot),
        commitmentProofCompact,
        additionProofCompact
      ]
      const receipt = await sendTransaction(
        web3,
        mvppt._address,
        mvppt.methods.deposit(...params).encodeABI(),
        5000000,
        accounts.bob
      )

      assert(receipt.status)

      server.add(data.cm)

    }

    log('done.')

    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)

    await util.prompt()

    const askPrice = new BN("50000")

    // Set up trade
    const expected_coin = sampleCoin(a_sk_alice, askPrice)
    const cm = await server.cm(
      a_pk_alice,
      expected_coin.rho,
      expected_coin.r,
      expected_coin.v.toString()
    )
    coins.push()

    // log(`Expected hidden note: ${cm}`)

    write([
      `Alice: Deploying offer smart contract (offer to sell car ${carId} for `,
      `payment output ${cm})...`
    ].join(''))
    const cm_packed = await util.pack256Bits(cm)
    const tradeContract = await util.deployContract(
      web3,
      artefacts.HiddenPriceCarTrade,
      [carToken._address, mvppt._address, carId, cm_packed[0], cm_packed[1]],
      accounts.alice
    )
    write('done.\n')

    await sendTransaction(
      web3,
      carToken._address,
      carToken.methods.approve(tradeContract._address, carId).encodeABI(),
      5000000,
      accounts.alice
    )

    await util.prompt()

    write([
      `Bob: Send a private payment that generates the payment output ${cm} `,
      `and references the smart contract (and consequently finalises the `,
      `trade)...`
    ].join(''))

    const change_coin = sampleCoin(a_sk_bob, coins[0].v.add(coins[1].v).sub(expected_coin.v))


    const params = [
      "0",
      coins[0].a_sk,
      coins[0].rho,
      coins[0].r,
      coins[0].v.toString(),
      "1",
      coins[1].a_sk,
      coins[1].rho,
      coins[1].r,
      coins[1].v.toString(),
      a_pk_alice,
      expected_coin.rho,
      expected_coin.r,
      expected_coin.v.toString(),
      a_pk_bob,
      change_coin.rho,
      change_coin.r,
      change_coin.v.toString(),
      tradeContract._address
    ]
    const timestampStart = Date.now()
    const res = await server.prepare_transfer(params)
    const proofDuration = Date.now() - timestampStart

    const sn0Packed = await util.pack256Bits(res.input_0_sn)
    const sn1Packed = await util.pack256Bits(res.input_1_sn)
    const cm0Packed = await util.pack256Bits(res.output_0_cm)
    const cm1Packed = await util.pack256Bits(res.output_1_cm)

    // log(`Hidden notes: ${res.output_0_cm}, ${res.output_1_cm}`)
    // log('cm actual', await util.pack256Bits(res.output_0_cm))

    // TODO this should be simulated (and the actual addition should come
    // after the transaction is processed)
    await server.add(res.output_0_cm)
    const add_res = await server.add(res.output_1_cm)

    const newRoot = await util.pack256Bits(add_res.newRoot)

    const transferProofCompact = flattenProof(res.transfer_proof)
    const transferParams = [
      sn0Packed,
      sn1Packed,
      cm0Packed,
      cm1Packed,
      newRoot,
      tradeContract._address,
      transferProofCompact
    ]
    const x = mvppt.methods.transfer(...transferParams)
    const data = x.encodeABI()

    const receipt = await sendTransaction(web3, mvppt._address, data, 5000000, accounts.bob)

    write('done.\n')

    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)

    await util.prompt()

    // WITHDRAW
    // Alice
    write('Alice: Withdraw 50000...')
    {
      const wdRes = await server.prepare_withdrawal(
        "2",
        expected_coin.a_sk,
        expected_coin.rho,
        expected_coin.r,
        expected_coin.v.toString(),
        accounts.alice.address
      )
      const {sn, proof} = wdRes
      const snPacked = await util.pack256Bits(sn)
      const x = mvppt.methods.withdraw(expected_coin.v.toString(), snPacked, ...proof)
      const data = x.encodeABI()
      const receipt = await sendTransaction(web3, mvppt._address, data, 5000000, accounts.alice)
    }
    write('done.\n')

    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)


    await util.prompt()

    write('Bob: Withdraw 20000...')
    {
      const wdRes = await server.prepare_withdrawal(
        "3",
        change_coin.a_sk,
        change_coin.rho,
        change_coin.r,
        change_coin.v.toString(),
        accounts.bob.address
      )

      const {sn, proof} = wdRes
      const snPacked = await util.pack256Bits(sn)
      const x = mvppt.methods.withdraw(change_coin.v.toString(), snPacked, ...proof)
      const data = x.encodeABI()
      const receipt = await sendTransaction(web3, mvppt._address, data, 5000000, accounts.bob)
    }
    write('done.\n')

    await printState(dollarCoin, carToken, accountAddresses, accountNames, carIds)
  })

})