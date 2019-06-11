'use strict'

const _ = require('lodash')
const bip39 = require('bip39')
const fs = require('fs')
const hdkey = require('ethereumjs-wallet/hdkey')
const BN = require('bn.js')
const assert = require('assert')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const u = require('../util')
const signTransaction = require('../eth/sign-transaction')
const compileContracts = require('../test/helpers/compile-contracts')
const makePlatformState = require('../platform-state')
const makeSecretStore = require('../secret-store')
const mnemonics = require('./mnemonics')
const log4js = require('log4js')
const initEventHandlers = require('./init-event-handlers')
const demoUtil = require('./util')
const serverPorts = require('./server-ports')
const mtServerPorts = require('./mt-server-ports')

const logger = log4js.getLogger()
logger.level = process.env.LOG_LEVEL || 'info'

process.on('unhandledRejection', error => {
  logger.error(error)
  process.exit(1)
})

async function run() {
  const who = process.argv[2]
  if ([ 'alice', 'bob', 'carol' ].indexOf(who) === -1) {
    console.log('Need parameter "alice", "bob" or "carol".')
    process.exit(1)
  }

  const platformState = await makePlatformState(mtServerPorts[who])
  const web3 = testUtil.initWeb3()
  const k0Eth = await makeEthPlatform(web3, u.hex2buf(addresses.MVPPT))

  const secretStoreData = require(`./${who}.secrets.json`)
  const secretStore = makeSecretStore(
    u.hex2buf(secretStoreData.privateKey),
    u.hex2buf(secretStoreData.publicKey)
  )
  console.log('private key', u.buf2hex(secretStore.getASk()))
  console.log('public key', u.buf2hex(secretStore.getAPk()))
  initEventHandlers(platformState, secretStore, k0Eth)

  const k0 = await makeK0(serverPorts[who])

  const artefacts = await compileContracts()
  const dollarCoin = new web3.eth.Contract(
    artefacts.DollarCoin.abi,
    addresses.DollarCoin
  )

  const mnemonic = mnemonics[who]
  const seed = bip39.mnemonicToSeed(mnemonic)
  const root = hdkey.fromMasterSeed(seed)
  const path = "m/44'/60'/0'/0/0" // eslint-disable-line
  const wallet = root.derivePath(path).getWallet()

  const values = _.times(3, () => new BN(_.random(50).toString() + '000'))

  const total = values.reduce((acc, el) => acc.add(el), new BN('0'))

  await demoUtil.prompt()

  const platformRoot = await k0Eth.merkleTreeRoot()
  console.log(`Platform Merkle tree root: ${u.buf2hex(platformRoot)}`)
  const localRoot = await platformState.merkleTreeRoot()
  console.log(`Local Merkle tree root: ${u.buf2hex(localRoot)}`)
  assert(localRoot.equals(platformRoot))

  // approve
  const approveTx = await signTransaction(
    web3,
    u.hex2buf(dollarCoin._address),
    u.hex2buf(
      dollarCoin.methods.approve(addresses.MVPPT, total.toString()).encodeABI()
    ),
    5000000,
    wallet.getASk()
  )
  await web3.eth.sendSignedTransaction(u.buf2hex(approveTx))

  for (let i = 0; i < values.length; i++) {
    const v = values[i]

    const data = await k0.prepareDeposit(platformState, secretStore.getAPk(), v)

    await secretStore.addNoteInfo(data.cm, data.a_pk, data.rho, data.r, v)

    const depositTx = await k0Eth.deposit(
      wallet.getASk(),
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

  fs.writeFileSync(`${who}.secrets.json`, JSON.stringify(secretStore.spit()))
}

(async () => {
  try {
    await run()
    process.exit(0)
  } catch (err) {
    console.log(err)
    console.log('Deposit failed...')
    process.exit(1)
  }
})()
