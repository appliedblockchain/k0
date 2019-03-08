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

describe('Transparent trade', async () => {

  let web3, accounts, accountNames, carIds, tokenMaster, carManufacturer,
    artefacts, dollarCoin, carToken, k0Coin, client

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

    server = makeClient()
    await server.ready()
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
    k0Coin = await util.deployContract(
      web3,
      artefacts.MVPPT,
      [
        dollarCoin._address,
        ...verifierAddresses,
        await util.pack256Bits(initialRoot)
      ]
    )
  })

  function sampleCoin(v) {
    assert(BN.isBN(v))
    return {
      rho: util.randomBytes(32),
      r: util.randomBytes(48),
      a_sk: util.randomBytes(32),
      v
    }
  }

  it('Full cycle', async () => {
    // bob: Pay in 2x
    printState(dollarCoin, carToken, accounts, accountNames, carIds)


    const coins = [
      sampleCoin(new BN("30000")),
      sampleCoin(new BN("40000"))
    ]
    for (let i = 0; i < 2; i++) {

      const c = coins[i]
      await sendTransaction(
        web3,
        dollarCoin._address,
        dollarCoin.methods.approve(k0Coin._address, c.v.toString()).encodeABI(),
        5000000,
        accounts.bob
      )

      const a_pk = await server.prf_addr(c.a_sk)
      console.log(a_pk)
      const data = await server.prepare_deposit(a_pk, c.rho, c.r, c.v.toString())
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
        k0Coin._address,
        k0Coin.methods.deposit(...params).encodeABI(),
        5000000,
        accounts.bob
      )

      assert(receipt.status)

      server.add(data.cm)

    }

  })
})