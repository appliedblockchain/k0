'use strict'

const BN = require('bn.js')
const _ = require('lodash')
const bip39 = require('bip39')
const crypto = require('crypto')
const log4js = require('log4js')
const { expect } = require('chai')
const waitPort = require('wait-port')
const jayson = require('jayson/promise')
const hdkey = require('ethereumjs-wallet/hdkey')

const u = require('../../util')
const makeK0 = require('../../k0')
const testUtil = require('../util')
const makeEthPlatform = require('../../eth')
const serverReady = require('../../client/ready')
const makeSecretStore = require('../../secret-store')
const sendTransaction = require('../../send-transaction')
const makePlatformState = require('../../platform-state')
const compileContracts = require('../helpers/compile-contracts')
const initEventHandlers = require('../../demo/init-event-handlers')
const signTransaction = require('../../eth/sign-transaction')

const assert = require('assert')

const platformPorts = [ 4100, 5100, 6100 ]

let web3

after(() => {
  web3.currentProvider.connection.close()
})

describe('Ethereum integration test replicating the K0 demo', () => {

  let alice, bob, carol // web3 accounts, and secretKeys
  let artefacts
  let addresses // contract and user addresses, indexed by name
  let k0, k0Eth
  let verifierAddresses, platformState1, platformState2, platformState3
  let dollarCoin, carToken

  const numInitialNotes = 2

  // Make the accounts, deploy the contracts
  before(async () => {
    const logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'

    process.on('unhandledRejection', error => {
      logger.error(error)
      process.exit(1)
    })

    await waitPort({ port: 4000 })
    await waitPort({ port: 8546 })
    await waitPort({ port: platformPorts[0] }) // parity
    await waitPort({ port: platformPorts[1] })
    await waitPort({ port: platformPorts[2] })

    web3 = testUtil.initWeb3()
    // DollarCoin minter
    const tokenMaster = web3.eth.accounts.create()

    // CarToken minter
    const carManufacturer = web3.eth.accounts.create()

    // contract artefacts
    artefacts = await compileContracts()

    // ERC-20 payment token
    dollarCoin = await testUtil.deployContract(web3, artefacts.DollarCoin, [], tokenMaster)

    // ERC-721 token representing cars
    carToken = await testUtil.deployContract(web3, artefacts.CarToken, [], carManufacturer)

    verifierAddresses = await Promise.all([
      'CommitmentVerifier',
      'AdditionVerifier',
      'TransferVerifier',
      'WithdrawalVerifier'
    ].map(async name => {
      const contract = await testUtil.deployContract(web3, artefacts[name])
      return contract._address
    }))

    platformState1 = await makePlatformState(platformPorts[0])
    platformState2 = await makePlatformState(platformPorts[1])
    platformState3 = await makePlatformState(platformPorts[2])

    await u.wait(500)

    const initialRoot = await platformState1.merkleTreeRoot()

    // Deploying coin contract
    const mvppt = await testUtil.deployContract(
      web3,
      artefacts.MVPPT,
      [
        dollarCoin._address,
        ...verifierAddresses,
        await testUtil.pack256Bits(u.buf2hex(initialRoot))
      ]
    )

    await serverReady(jayson.client.http({ port: 4000 }))

    const moneyShower = await testUtil.deployContract(web3, artefacts.MoneyShower)

      ;([ alice, bob, carol ] = _.times(3, () => {
      const mnemonic = bip39.generateMnemonic()
      const seed = bip39.mnemonicToSeed(mnemonic)
      const root = hdkey.fromMasterSeed(seed)
        const path = "m/44'/60'/0'/0/0" // eslint-disable-line
      const wallet = root.derivePath(path).getWallet()
      return { mnemonic, wallet }
    }))

    // Make some money
    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods.mint(tokenMaster.address, '1000000000000000').encodeABI(),
      5000000,
      tokenMaster
    )



    // Money to the people
    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods.approve(
        moneyShower._address, web3.utils.toWei((numInitialNotes).toString())
      ).encodeABI(), 5000000, tokenMaster)

    await sendTransaction(
      web3,
      moneyShower._address,
      moneyShower.methods.transfer(
        dollarCoin._address,
        _.map([ alice, bob ], x => x.wallet.getAddressString()),
        _.times(2, () => '1000000000000')
      ).encodeABI(),
      5000000,
      tokenMaster
    )

    // Make a car for Alice
    const carId = 1
    await sendTransaction(
      web3,
      carToken._address,
      carToken.methods.mint(alice.wallet.getAddressString(), carId).encodeABI(),
      5000000,
      carManufacturer
    )

    addresses = {
      DollarCoin: dollarCoin._address,
      CarToken: carToken._address,
      MVPPT: mvppt._address,
      alice: alice.wallet.getAddressString(),
      bob: bob.wallet.getAddressString(),
      carol: carol.wallet.getAddressString()
    }

    k0Eth = await makeEthPlatform(
      web3,
      u.hex2buf(addresses.MVPPT)
    )

    alice.secretKey = crypto.randomBytes(32)
    bob.secretKey = crypto.randomBytes(32)
    carol.secretKey = crypto.randomBytes(32)

    k0 = await makeK0(4000)
    alice.publicKey = await k0.prfAddr(alice.secretKey)
    bob.publicKey = await k0.prfAddr(bob.secretKey)
    carol.publicKey = await k0.prfAddr(carol.secretKey)

    alice.secretStore = makeSecretStore(alice.secretKey, alice.publicKey)

    bob.secretStore = makeSecretStore(bob.secretKey, bob.publicKey)

    carol.secretStore = makeSecretStore(carol.secretKey, carol.publicKey)

    initEventHandlers(platformState1, alice.secretStore, k0Eth)
    initEventHandlers(platformState2, bob.secretStore, k0Eth)
    initEventHandlers(platformState3, carol.secretStore, k0Eth)

    console.log('before: INITIALIZED Secrets, ')
  })

  async function checkRootsConsistency() {
    const ethRoot = await k0Eth.merkleTreeRoot()
    const root1 = await platformState1.merkleTreeRoot()
    const root2 = await platformState2.merkleTreeRoot()
    const root3 = await platformState3.merkleTreeRoot()

    assert(ethRoot.equals(root1))
    assert(ethRoot.equals(root2))
    assert(ethRoot.equals(root3))
  }

  // Consume  $coin in exchange for CMs
  async function approveAndDeposit(wallet, secretStore, platformState, values) {
    // aprove an amounts to mvptt
    const total = values.reduce((acc, el) => acc.add(el), new BN('0'))

    const approveTx = await signTransaction(
      web3,
      u.hex2buf(dollarCoin._address),
      u.hex2buf(dollarCoin.methods.approve(addresses.MVPPT, total.toString()).encodeABI()),
      5000000,
      wallet.getPrivateKey()
    )
    await web3.eth.sendSignedTransaction(u.buf2hex(approveTx))

    for (let i = 0; i < values.length; i++) {
      const v = values[i]

      const data = await k0.prepareDeposit(platformState, secretStore, v)

      await secretStore.addNoteInfo(data.cm, data.a_pk, data.rho, data.r, v)

      const depositTx = await k0Eth.deposit(
        wallet.getPrivateKey(),
        v,
        data.k,
        data.cm,
        data.nextRoot,
        data.commitmentProofAffine,
        data.additionProofAffine
      )

      await web3.eth.sendSignedTransaction(u.buf2hex(depositTx))
      await u.wait(2000)
    }
  }

  let values
  it('Can mint the CMS', async () => {
    await u.wait(2000)
    // DEPOSIT TEST
    values = _.times(numInitialNotes, () => new BN(_.random(50).toString() + '000'))
    await approveAndDeposit(alice.wallet, alice.secretStore, platformState1, values)
    // values = _.times(3, () => new BN(_.random(50).toString() + '000'))
    // await approveAndDeposit(bob.wallet, bob.secretStore, platformState2, values)
    await checkRootsConsistency()

    // check that we have now 6 cm in the merkle tree
    expect(await platformState1.currentState().cmList.length).to.equal(numInitialNotes)
  })

  async function getInputNote(platformState, secretStore, index) {
    const cm = platformState.cmAtIndex(new BN(index))
    const info = secretStore.getNoteInfo(cm)

    return {
      address: new BN(index),
      cm,
      ...info
    }
  }

  /**
   * convert data to eth compatible format
   * @param {} a_pk
   * @param {*} rho
   * @param {*} r
   * @param {*} v
   */
  function makeData(a_pk, rho, r, v) {
    u.checkBuf(a_pk, 32)
    u.checkBuf(rho, 32)
    u.checkBuf(r, 48)
    u.checkBN(v)
    return Buffer.concat([ a_pk, rho, r, v.toBuffer('le', 64) ])
  }

  it('allows alice to transfer funds to bob', async () => {
    await checkRootsConsistency()
    const in0 = await getInputNote(platformState1, alice.secretStore, 0)
    const in1 = await getInputNote(platformState1, alice.secretStore, 1)

    console.log({ in0, in1 })

    const totalValue = in0.v.add(in1.v)



    const out0 = {
      a_pk: bob.secretStore.getPublicKey(),
      v: totalValue,
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48)
    }
    const out1 = {
      a_pk: alice.secretStore.getPublicKey(),
      v: new BN('0'),
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48)
    }

    console.log({ platformRoot: await platformState1.merkleTreeRoot(), ethRoot: await k0Eth.merkleTreeRoot() })
    console.log({ totalValue: totalValue.toString(), in0: in0.v.toString(), in1: in1.v.toString() })


    const callee = u.hex2buf(u.ZERO_ADDRESS) // zero address since we are not trading
    // does not throw
    const transferData = await k0.prepareTransfer(
      platformState1,
      alice.secretStore,
      new BN(0), // cms index
      new BN(1),
      out0,
      out1,
      u.hex2buf(u.ZERO_ADDRESS),
      callee
    )

    alice.secretStore.addSNToNote(in0.cm, transferData.input_0_sn)
    alice.secretStore.addSNToNote(in1.cm, transferData.input_1_sn)
    alice.secretStore.addNoteInfo(transferData.output_0_cm, out0.a_pk, out0.rho, out0.r, out0.v)
    alice.secretStore.addNoteInfo(transferData.output_1_cm, out1.a_pk, out1.rho, out1.r, out1.v)


    const rootBefore = await platformState1.merkleTreeRoot()
    const labelBefore = platformState1.currentStateLabel()
    const tmpLabel = ('temporary_mt_addition_' + crypto.randomBytes(16).toString('hex'))
    await platformState1.add(tmpLabel, [], [ transferData.output_0_cm, transferData.output_1_cm ])
    const newRoot = await platformState1.merkleTreeRoot()
    await platformState1.rollbackTo(labelBefore)

    const finalRoot = await platformState1.merkleTreeRoot()
    assert(finalRoot.equals(rootBefore))

    const out_0_data = makeData(out0.a_pk, out0.rho, out0.r, out0.v)
    const out_1_data = makeData(out1.a_pk, out1.rho, out1.r, out1.v)


    const ethParams = [
      crypto.randomBytes(32),
      transferData.input_0_sn,
      transferData.input_1_sn,
      transferData.output_0_cm,
      transferData.output_1_cm,
      out_0_data,
      out_1_data,
      newRoot,
      callee,
      transferData.proofAffine
    ]
    const tx = await k0Eth.transfer(...ethParams)

    const receipt = await web3.eth.sendSignedTransaction(u.buf2hex(tx))
    expect(receipt.status).to.equal(true)
  })
})
