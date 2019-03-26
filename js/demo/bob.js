'use strict'
const bip39 = require('bip39')
const hdkey = require('ethereumjs-wallet/hdkey')
const BN = require('bn.js')
const crypto = require('crypto')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const u = require('../util')
const mnemonics = require('./mnemonics')
const signTransaction = require('../eth/sign-transaction')
const compileContracts = require('../test/helpers/compile-contracts')
const printState = require('./print-state')
const makeState = require('../state')

async function run() {
  const web3 = testUtil.initWeb3()
  const k0Eth = await makeEthPlatform(
    web3,
    u.hex2buf(addresses.MVPPT)
  )

  const artefacts = await compileContracts()
  const dollarCoin = new web3.eth.Contract(
    artefacts.DollarCoin.abi,
    addresses.DollarCoin
  )
  const a_sk = crypto.randomBytes(32)
  const state = await makeState(parseInt(process.env.SERVER_PORT || '5000', 10))
  console.log('root before', await state.root())
  await state.reset()
  console.log('root after', await state.root())
  const proverPort = parseInt(process.env.SERVER_PORT || '5000', 10)
  const k0 = await makeK0(a_sk, k0Eth, state, proverPort)
  const v = new BN('50000')
  const data = await k0.prepareDeposit(v)
  console.log(data)

  //web3.eth.accounts.signTransaction(tx, privateKey [, callback]);
  const mnemonic = mnemonics.bob
  const seed = bip39.mnemonicToSeed(mnemonic)
  const root = hdkey.fromMasterSeed(seed)
  const path = "m/44'/60'/0'/0/0"
  const wallet = root.derivePath(path).getWallet()

  printState(dollarCoin, null, [addresses.alice, addresses.bob], ['alice', 'bob'], [])

  // approve
  const approveTx = await signTransaction(
    web3,
    u.hex2buf(dollarCoin._address),
    u.hex2buf(dollarCoin.methods.approve(addresses.MVPPT, v.toString()).encodeABI()),
    5000000,
    wallet.getPrivateKey()
  )
  await web3.eth.sendSignedTransaction(u.buf2hex(approveTx))

  console.log('approve done')

  const depositTx = await k0Eth.deposit(
    wallet.getPrivateKey(),
    v,
    data.k,
    data.cm,
    data.nextRoot,
    data.commitmentProof,
    data.additionProof
  )
  const receipt = await web3.eth.sendSignedTransaction(u.buf2hex(depositTx))
}

run().then(console.log).catch(console.log)
