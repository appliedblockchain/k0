'use strict'

const bip39 = require('bip39')
const compileContracts = require('../test/helpers/compile-contracts')
const crypto = require('crypto')
const fs = require('fs')
const testUtil = require('../test/util')
const _ = require('lodash')
const hdkey = require('ethereumjs-wallet/hdkey')
const addressOfPublicKey = require('@appliedblockchain/helpers/address-of-public-key')
const sendTransaction = require('../send-transaction')
const makePlatformState = require('../platform-state')
const u = require('../util')
const makeK0 = require('../k0')

async function run() {

  const web3 = testUtil.initWeb3()
  // DollarCoin minter
  const tokenMaster = web3.eth.accounts.create()

  // CarToken minter
  const carManufacturer = web3.eth.accounts.create()

  // contract artefacts
  const artefacts = await compileContracts()

  // ERC-20 payment token
  const dollarCoin = await testUtil.deployContract(
    web3, artefacts.DollarCoin, [], tokenMaster)

  // ERC-721 token representing cars
  const carToken = await testUtil.deployContract(
    web3, artefacts.CarToken, [], carManufacturer)

  const verifierAddresses = await Promise.all([
    'CommitmentVerifier',
    'AdditionVerifier',
    'TransferVerifier',
    'WithdrawalVerifier'
  ].map(async name => {
    const contract = await testUtil.deployContract(web3, artefacts[name])
    return contract._address
  }))

  const platformState = await makePlatformState()
  const initialRoot = await platformState.merkleTreeRoot()
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

  const [ alice, bob ] = _.times(2, () => {
    const mnemonic = bip39.generateMnemonic()
    const seed = bip39.mnemonicToSeed(mnemonic)
    const root = hdkey.fromMasterSeed(seed)
    const path = "m/44'/60'/0'/0/0"
    const wallet = root.derivePath(path).getWallet()
    return { mnemonic, wallet }
  })

  // Make some money
  await sendTransaction(
    web3,
    dollarCoin._address,
    dollarCoin.methods.mint(tokenMaster.address, "1000000000000000").encodeABI(),
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
    ).encodeABI(),
    5000000,
    tokenMaster
  )

  await sendTransaction(
    web3,
    moneyShower._address,
    moneyShower.methods.transfer(
      dollarCoin._address,
      _.map([ alice, bob ], x => x.wallet.getAddressString()),
      _.times(2, () => "1000000000000")
    ).encodeABI(),
    5000000,
    tokenMaster
  )

  fs.writeFileSync('artefacts.json', JSON.stringify(artefacts))
  fs.writeFileSync('addresses.json', JSON.stringify({
    DollarCoin: dollarCoin._address,
    CarToken: carToken._address,
    MVPPT: mvppt._address,
    alice: alice.wallet.getAddressString(),
    bob: bob.wallet.getAddressString()
  }))
  fs.writeFileSync('mnemonics.json', JSON.stringify({
    alice: alice.mnemonic,
    bob: bob.mnemonic
  }))

  const aliceSecretKey = crypto.randomBytes(32)
  const bobSecretKey = crypto.randomBytes(32)

  const k0 = await makeK0()
  const alicePublicKey = await k0.prfAddr(aliceSecretKey)
  const bobPublicKey = await k0.prfAddr(bobSecretKey)
  fs.writeFileSync('public-keys.json', JSON.stringify({
    alice: u.buf2hex(alicePublicKey),
    bob: u.buf2hex(bobPublicKey)
  }))
  fs.writeFileSync('alice.secrets.json', JSON.stringify({
    privateKey: u.buf2hex(aliceSecretKey),
    publicKey: u.buf2hex(alicePublicKey)
  }))
  fs.writeFileSync('bob.secrets.json', JSON.stringify({
    privateKey: u.buf2hex(bobSecretKey),
    publicKey: u.buf2hex(bobPublicKey)
  }))



}

run().then(console.log).catch(console.log)
