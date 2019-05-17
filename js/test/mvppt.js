'use strict'

const _ = require('lodash')
const assert = require('assert')
const jayson = require('jayson/promise')
const util = require('./util')
const sendTransaction = require('../send-transaction')
const compileContracts = require('./helpers/compile-contracts')
const serverReady = require('../client/ready')
const flattenProof = require('../eth/flatten-proof')

const numInitialCoins = 2

describe('Minimum viable private payment token', function () {

  this.timeout(4 * 3600 * 1000)
  let web3, erc20, MVPPT, tokenMaster, depositors, mtEngine, proverEngine

  async function printBalances(addresses) {
    addresses = [ MVPPT._address, ...addresses ]
    console.log('Balances:')
    for (let i = 0; i < addresses.length; i++) {
      const balance = await erc20.methods.balanceOf(addresses[i]).call()
      console.log(addresses[i], balance)
    }
  }

  before(async () => {

    web3 = util.initWeb3()
    tokenMaster = web3.eth.accounts.create()
    depositors = _.times(numInitialCoins, () => web3.eth.accounts.create())

    const artefacts = await compileContracts()

    erc20 = await util.deployContract(web3, artefacts.DollarCoin, [], tokenMaster)
    const moneyShower = await util.deployContract(web3, artefacts.MoneyShower)

    // Make some money
    await sendTransaction(
      web3,
      erc20._address,
      erc20.methods.mint(tokenMaster.address, '1000000000000000').encodeABI(),
      5000000,
      tokenMaster
    )

    // Money to the people
    await sendTransaction(
      web3,
      erc20._address,
      erc20.methods.approve(
        moneyShower._address, web3.utils.toWei((numInitialCoins).toString())
      ).encodeABI(),
      5000000,
      tokenMaster
    )

    await sendTransaction(
      web3,
      moneyShower._address,
      moneyShower.methods.transfer(
        erc20._address,
        _.map(depositors, 'address'),
        _.times(depositors.length, () => '1000000000000')
      ).encodeABI(),
      5000000,
      tokenMaster
    )
    const merklePort = parseInt(process.env.MERKLE_PORT || '4100', 10)
    mtEngine = jayson.client.http({ port: merklePort })

    const proverPort = parseInt(process.env.PROVER_PORT || '4000', 10)
    proverEngine = jayson.client.http({ port: proverPort })

    console.log({ merklePort, proverPort })

    await serverReady(mtEngine, 'Merkle Tree')
    await serverReady(proverEngine, 'Prover')


    await mtEngine.request('reset', [])
    console.log('MT ENGINE RESETTED')
    const initialRootResponse = await mtEngine.request('root', [])

    console.log('here')
    const initialRoot = initialRootResponse.result
    const verifierAddresses = await Promise.all([
      'CommitmentVerifier',
      'AdditionVerifier',
      'TransferVerifier',
      'WithdrawalVerifier'
    ].map(async name => {
      const contract = await util.deployContract(web3, artefacts[name])
      return contract._address
    }))
    MVPPT = await util.deployContract(
      web3,
      artefacts.MVPPT,
      [
        erc20._address,
        ...verifierAddresses,
        await util.pack256Bits(initialRoot)
      ]
    )
  })

  it('Lifecycle', async function () {
    const coins = [
        {
          rho: '0x0101010101010101010101010101010101010101010101010101010101010101',
          r: '0x020202020202020202020202020202020202020202020202020202020202020202020202020202020202020202020202',
          a_sk: '0x0303030303030303030303030303030303030303030303030303030303030303',
          v: '200000000000'
        },
        {
          rho: '0x1111111111111111111111111111111111111111111111111111111111111111',
          r: '0x121212121212121212121212121212121212121212121212121212121212121212121212121212121212121212121212',
          a_sk: '0x1313131313131313131313131313131313131313131313131313131313131313',
          v: '500000000000'
        },
        {
          rho: '0x2121212121212121212121212121212121212121212121212121212121212121',
          r: '0x222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222',
          a_sk: '0x2323232323232323232323232323232323232323232323232323232323232323',
          v: '300000000000'
        },
        {
          rho: '0x3131313131313131313131313131313131313131313131313131313131313131',
          r: '0x323232323232323232323232323232323232323232323232323232323232323232323232323232323232323232323232',
          a_sk: '0x3333333333333333333333333333333333333333333333333333333333333333',
          v: '400000000000'
        }
      ],
      commitmentProvingTimes = [],
      withdrawalProvingTimes = []

    printBalances(_.map(depositors, 'address'))

    const initRootResponse = await mtEngine.request('root', [])
    console.log('Initial Merkle tree root', initRootResponse.result)

    for (let i = 0; i < numInitialCoins; i++) {
      const account = depositors[i]

      // const v = (_.random(1, 99) * 100000000).toString()
      const { a_sk, rho, r, v } = coins[i]
      await sendTransaction(
        web3,
        erc20._address,
        erc20.methods.approve(MVPPT._address, v).encodeABI(),
        5000000,
        account
      )

      const aPkResponse = await mtEngine.request('prf_addr', [ a_sk ])
      const a_pk = aPkResponse.result

      coins[i] = { a_sk, rho, r, v }

      const timestampStart = Date.now()
      const response = await proverEngine.request('prepare_deposit', [ a_pk, rho, r, v ]) // check demo
      const proofDuration = Date.now() - timestampStart

      console.log({ response })

      // const contractRoot =
      await Promise.all([
        MVPPT.methods.root(0).call(),
        MVPPT.methods.root(1).call()
      ])

      const serverRootResponse = await mtEngine.request('root', [])
      console.log(serverRootResponse)
      // const serverRoot =
      await util.pack256Bits(serverRootResponse.result)

      const data = response.result

      console.log(data.commitmentProof)
      const commitmentProofCompact = flattenProof(data.commitmentProof)
      const additionProofCompact = flattenProof(data.additionProof)

      const params = [
        v,
        await util.pack256Bits(data.k),
        await util.pack256Bits(data.cm),
        await util.pack256Bits(data.nextRoot),
        commitmentProofCompact,
        additionProofCompact
      ]
      console.log({ params })

      const txData = MVPPT.methods.deposit(
        ...params
      ).encodeABI()
      const receipt = await sendTransaction(web3, MVPPT._address, txData, 5000000, account)

      assert(receipt.status)

      const depositResponse = await mtEngine.request('add', [ data.cm ])
      console.log(`Added leaf ${i}: ${data.cm}`)
      console.log(`New root: ${depositResponse.result.newRoot}, should be ${data.nextRoot}`)
      assert(depositResponse.result.newRoot === data.nextRoot)
      commitmentProvingTimes.push(proofDuration)

      const timesSum = commitmentProvingTimes.reduce((acc, val) => acc + val)
      const avg = timesSum / commitmentProvingTimes.length

      console.log([
        'Duration of commitment proving operation:',
        `${Math.round(proofDuration / 1000)}s`,
        `(avg: ${Math.round(avg / 1000)}s)`
      ].join(' '))

      await printBalances(_.map(depositors, 'address'))
    }

    // TRANSFER
    const transferors = _.times(numInitialCoins, () => web3.eth.accounts.create())
    for (let i = 0; i < 1; i += 2) {
      const inputs = [ coins[i], coins[i + 1] ],
        outputs = [ coins[i + 2], coins[i + 3] ]

      for (let j = 0; j < 2; j++) {
        const a_sk = util.randomBytesHex(32)
        const rho = util.randomBytesHex(32)
        const r = util.randomBytesHex(48)
        const v = 50000000000
        coins.push({ a_sk, rho, r, v })
        outputs.push({ a_sk, rho, r, v })
      }


      const a_pk_out = await Promise.all(outputs.map(async o => { // eslint-disable-line
        const aPkResponse = await mtEngine.request('prf_addr', [ o.a_sk ])
        return aPkResponse.result
      }))

      const callee = '0x0000000000000000000000000000000000000000'
      const params = [
        i.toString(10),
        inputs[0].a_sk,
        inputs[0].rho,
        inputs[0].r,
        inputs[0].v.toString(),
        (i + 1).toString(10),
        inputs[1].a_sk,
        inputs[1].rho,
        inputs[1].r,
        inputs[1].v.toString(),
        a_pk_out[0],
        outputs[0].rho,
        outputs[0].r,
        outputs[0].v.toString(),
        a_pk_out[1],
        outputs[1].rho,
        outputs[1].r,
        outputs[1].v.toString(),
        callee
      ]

      // const timestampStart = Date.now()
      const response = await mtEngine.request('prepare_transfer', params)
      // const proofDuration = Date.now() - timestampStart

      const res = response.result

      console.log(response)
      const sn0Packed = await util.pack256Bits(res.input_0_sn)
      const sn1Packed = await util.pack256Bits(res.input_1_sn)
      const cm0Packed = await util.pack256Bits(res.output_0_cm)
      const cm1Packed = await util.pack256Bits(res.output_1_cm)


      // TODO this should be simulated (and the actual addition should come
      // after the transaction is processed)
      await mtEngine.request('add', [ res.output_0_cm ])
      const addResponse = await mtEngine.request('add', [ res.output_1_cm ])
      const newRoot = await util.pack256Bits(addResponse.result.newRoot)

      const transferProofCompact = flattenProof(res.transfer_proof)
      const transferParams = [
        sn0Packed,
        sn1Packed,
        cm0Packed,
        cm1Packed,
        newRoot,
        callee,
        transferProofCompact
      ]
      const x = MVPPT.methods.transfer(...transferParams)
      const data = x.encodeABI()

      const account = transferors[i]
      const receipt = await sendTransaction(web3, MVPPT._address, data, 5000000, account)
      console.log('Transfer successful?', receipt.status)


    }

    const withdrawers = _.times(numInitialCoins, () => web3.eth.accounts.create())

    for (let i = 0; i < withdrawers.length; i++) {

      const account = withdrawers[i]

      const address = numInitialCoins + i
      const { a_sk, rho, r, v } = coins[address]
      const timestampStart = Date.now()
      const params = [ address.toString(10), a_sk, rho, r, v.toString(), account.address ]
      const withdrawalProofResponse = await mtEngine.request(
        'prepare_withdrawal', params
      )
      const proofDuration = Date.now() - timestampStart
      const { sn, proof } = withdrawalProofResponse.result
      const snPacked = await util.pack256Bits(sn)
      const x = MVPPT.methods.withdraw(v, snPacked, ...proof)
      const data = x.encodeABI()
      const receipt = await sendTransaction(web3, MVPPT._address, data, 5000000, account)
      console.log('Withdrawal successful?', receipt.status)
      await printBalances(_.map([ ...depositors, ...withdrawers ], 'address'))
      withdrawalProvingTimes.push(proofDuration)

      const timesSum = withdrawalProvingTimes.reduce((acc, val) => acc + val)
      const avg = timesSum / withdrawalProvingTimes.length

      console.log([
        'Duration of withdrawal proving operation:',
        `${Math.round(proofDuration / 1000)}s`,
        `(avg: ${Math.round(avg / 1000)}s)`
      ].join(' '))
      console.log()
    }

  })
})
