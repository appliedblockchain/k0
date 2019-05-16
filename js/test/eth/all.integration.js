'use strict'

const BN = require('bn.js')
const _ = require('lodash')
const bip39 = require('bip39')
const crypto = require('crypto')
const log4js = require('log4js')
const hdkey = require('ethereumjs-wallet/hdkey')
const makeSecretStore = require('../../secret-store')

const u = require('../../util')
const { buf2hex } = u
const makeK0 = require('../../k0')
const testUtil = require('../util')
const makeEthPlatform = require('../../eth')
const sendTransaction = require('../../send-transaction')
const makePlatformState = require('../../platform-state')
const compileContracts = require('../helpers/compile-contracts')
const initEventHandlers = require('../../demo/init-event-handlers')

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
  let aliceSecretStore, bobSecretStore, carolSecretStore

  // Make the accounts, deploy the contracts
  before(async () => {
    const logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'

    process.on('unhandledRejection', error => {
      logger.error(error)
      process.exit(1)
    })


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

    const numInitialCoins = 2

    // Money to the people
    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods.approve(
        moneyShower._address, web3.utils.toWei((numInitialCoins).toString())
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

    const aliceSecretKey = crypto.randomBytes(32)
    const bobSecretKey = crypto.randomBytes(32)
    const carolSecretKey = crypto.randomBytes(32)

    k0 = await makeK0()
    const alicePublicKey = await k0.prfAddr(aliceSecretKey)
    const bobPublicKey = await k0.prfAddr(bobSecretKey)
    const carolPublicKey = await k0.prfAddr(carolSecretKey)


    aliceSecretStore = makeSecretStore(
      {
        publicKey: u.buf2hex(alicePublicKey),
        privateKey: u.buf2hex(aliceSecretKey)

      }
    )

    bobSecretStore = makeSecretStore(
      {
        publicKey: u.buf2hex(bobPublicKey),
        privateKey: u.buf2hex(bobSecretKey)

      }
    )

    carolSecretStore = makeSecretStore(
      {
        publicKey: u.buf2hex(carolPublicKey),
        privateKey: u.buf2hex(carolSecretKey)
      }
    )

    initEventHandlers(platformState1, aliceSecretStore, k0Eth)
    initEventHandlers(platformState2, bobSecretStore, k0Eth)
    initEventHandlers(platformState3, carolSecretStore, k0Eth)

    console.log('before: INITIALIZED Secrets, ')
  })

  async function checkRootsConsitency() {
    const ethRoot = await k0Eth.merkleTreeRoot()
    const root1 = await platformState1.merkleTreeRoot()
    const root2 = await platformState1.merkleTreeRoot()
    const root3 = await platformState1.merkleTreeRoot()

    assert(ethRoot.equals(root1))
    assert(ethRoot.equals(root2))
    assert(ethRoot.equals(root3))
  }

  it('works, happy path', async () => {
    const values = _.times(3, () => new BN(_.random(50).toString() + '000'))

    const total = values.reduce((acc, el) => acc.add(el), new BN('0'))
    console.log(`Notes values: ${values.map(v => v.toString()).join(' ,')} total: ${total.toString()}`)

    await checkRootsConsitency()

    // console.log({ r1:buf2hex(root1), r2:buf2hex(root2), r3:buf2hex(root3), r4: buf2hex(ethRoot) })
    // assert(buf2hex(root1) === buf2hex(root2) === buf2hex(root3), 'Unconsistent platform roots')
  })
})

