'use strict'

const _ = require('lodash')
const bip39 = require('bip39')
const fs = require('fs')
const hdkey = require('ethereumjs-wallet/hdkey')
const BN = require('bn.js')
const crypto = require('crypto')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const u = require('../util')
const signTransaction = require('../eth/sign-transaction')
const compileContracts = require('../test/helpers/compile-contracts')
const printState = require('./print-state')
const makePlatformState = require('../platform-state')
const makeSecretStore = require('../secret-store')
const mnemonics = require('./mnemonics')
const log4js = require('log4js')
const initEventHandlers = require('./init-event-handlers')
const demoUtil = require('./util')

const logger = log4js.getLogger()
logger.level = process.env.LOG_LEVEL || 'info'

process.on('unhandledRejection', error => {
  logger.error(error)
  process.exit(1)
})

const serverPorts = {
  alice: 4000,
  bob: 5000
}

const mtServerPorts = {
  alice: 4100,
  bob: 5100
}

async function run() {

  const who = process.argv[2]
  if (['alice', 'bob'].indexOf(who) === -1) {
    console.log('Need parameter "alice" or "bob".')
    process.exit(1)
  }
  const platformState = await makePlatformState(mtServerPorts[who])
  const web3 = testUtil.initWeb3()
  const k0Eth = await makeEthPlatform(
    web3,
    u.hex2buf(addresses.MVPPT)
  )

  const secretStoreData = require(`./${who}.secrets.json`)
  const secretStore = makeSecretStore(secretStoreData)
  console.log('private key', secretStore.getPrivateKey())
  console.log('public key', secretStore.getPublicKey())
  initEventHandlers(platformState, secretStore, k0Eth)

  const k0 = await makeK0(serverPorts[who])

  const artefacts = await compileContracts()
  const dollarCoin = new web3.eth.Contract(
    artefacts.DollarCoin.abi,
    addresses.DollarCoin
  )












  // const who = process.argv[2]
  // if (['alice', 'bob'].indexOf(who) === -1) {
  //   console.log('Need parameter "alice" or "bob".')
  //   process.exit(1)
  // }

  // const web3 = testUtil.initWeb3()
  // const k0Eth = await makeEthPlatform(
  //   web3,
  //   u.hex2buf(addresses.MVPPT)
  // )

  // const artefacts = await compileContracts()
  // const dollarCoin = new web3.eth.Contract(
  //   artefacts.DollarCoin.abi,
  //   addresses.DollarCoin
  // )
  // const platformState = await makePlatformState(mtServerPorts[who])
  // const secretStoreData = require(`./${who}.secrets.json`)
  // const secretStore = makeSecretStore(secretStoreData)

  // logger.info([
  //   `Public key ${u.buf2hex(secretStore.getPublicKey())}`,
  //   `private key ${u.buf2hex(secretStore.getPrivateKey())}`
  // ].join(''))

  // initEventHandlers(platformState, secretStore, k0Eth)

  // const k0 = await makeK0(serverPorts[who])


  //web3.eth.accounts.signTransaction(tx, privateKey [, callback]);
  const mnemonic = mnemonics[who]
  const seed = bip39.mnemonicToSeed(mnemonic)
  const root = hdkey.fromMasterSeed(seed)
  const path = "m/44'/60'/0'/0/0"
  const wallet = root.derivePath(path).getWallet()

  const values = _.times(7, () => new BN(_.random(50).toString() + '000'))
  console.log(values)

  const total = values.reduce((acc, el) => acc.add(el), new BN('0'))
  console.log(total.toString())

  console.log(await dollarCoin.methods.balanceOf(u.buf2hex(wallet.getAddress())).call())

  await demoUtil.prompt()
  printState(secretStore, k0Eth, platformState)
  // approve
  const approveTx = await signTransaction(
    web3,
    u.hex2buf(dollarCoin._address),
    u.hex2buf(dollarCoin.methods.approve(addresses.MVPPT, total.toString()).encodeABI()),
    5000000,
    wallet.getPrivateKey()
  )
  await web3.eth.sendSignedTransaction(u.buf2hex(approveTx))

  console.log('approval done!')
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
      data.commitmentProof,
      data.additionProof
    )
    const receipt = await web3.eth.sendSignedTransaction(u.buf2hex(depositTx))

    await u.wait(1000)
  }

  fs.writeFileSync(`${who}.secrets.json`, JSON.stringify(secretStore.spit()))
}

run().then(console.log).catch(console.log)