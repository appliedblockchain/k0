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
const makePlatformState = require('../platform-state')
const makeSecretStore = require('../secret-store')

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
  const mtServerPort = parseInt(process.env.MT_SERVER_PORT || '5100', 10)
  const platformState = await makePlatformState(mtServerPort)
  const secretStore = makeSecretStore(a_sk)
  await platformState.reset()

  k0Eth.on('deposit', async (txHash, cm, nextRoot) => {
    u.checkBuf(txHash, 32)
    u.checkBuf(cm, 32)
    u.checkBuf(nextRoot, 32)
    await platformState.add(u.buf2hex(txHash), [cm], [], nextRoot)
  })

  const proverPort = parseInt(process.env.SERVER_PORT || '5000', 10)
  const k0 = await makeK0(proverPort)
  //web3.eth.accounts.signTransaction(tx, privateKey [, callback]);
  const mnemonic = mnemonics.bob
  const seed = bip39.mnemonicToSeed(mnemonic)
  const root = hdkey.fromMasterSeed(seed)
  const path = "m/44'/60'/0'/0/0"
  const wallet = root.derivePath(path).getWallet()

  const v1 = new BN('50000')
  const v2 = new BN('50000')
  // approve
  const approveTx = await signTransaction(
    web3,
    u.hex2buf(dollarCoin._address),
    u.hex2buf(dollarCoin.methods.approve(addresses.MVPPT, v1.add(v2).toString()).encodeABI()),
    5000000,
    wallet.getPrivateKey()
  )
  await web3.eth.sendSignedTransaction(u.buf2hex(approveTx))


  const values = [v1, v2]

  for (let i = 0; i < 1; i++) {
    const v = values[i]

    const data = await k0.prepareDeposit(platformState, secretStore, v)
    await secretStore.addNoteInfo(data.cm, data.rho, data.r, v)

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
}

run().then(console.log).catch(console.log)
