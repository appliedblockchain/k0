'use strict'
const bip39 = require('bip39')
const hdkey = require('ethereumjs-wallet/hdkey')
const BN = require('bn.js')
const crypto = require('crypto')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const mnemonics = require('./mnemonics')
const signTransaction = require('../eth/sign-transaction')
const compileContracts = require('../test/helpers/compile-contracts')
const printState = require('./print-state')
const makePlatformState = require('../platform-state')
const makeSecretStore = require('../secret-store')
const { prompt } = require('./util')
const u = require('../util')
const inquirer = require('inquirer')
const publicKeysInput = require('./public-keys')
const transferMoney = require('./transfer-money')
const generatePaymentData = require('./generate-payment-data')
const initEventHandlers = require('./init-event-handlers')

process.on('unhandledRejection', error => {
  console.log(error)
  process.exit(1)
})

const publicKeys = {}
Object.keys(publicKeysInput).forEach(name => {
  publicKeys[name] = u.hex2buf(publicKeysInput[name])
})

const serverPorts = {
  alice: 4000,
  bob: 5000
}

const mtServerPorts = {
  alice: 4100,
  bob: 5100
}

const addressBookData = {}
addressBookData[addresses.alice] = 'Alice'
addressBookData[addresses.bob] = 'Bob'

function addressBook(address) {
  u.checkBuf(address, 20)
  return addressBookData[u.buf2hex(address)] || null
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

  const carToken = new web3.eth.Contract(
    artefacts.CarToken.abi,
    addresses.CarToken
  )

  const carIds = [ new BN('1') ]
  async function showState() {
    printState(secretStore, addressBook, k0Eth, platformState, carToken, carIds)
  }

  async function cycle() {
    const questions = [
      {
        type: 'list',
        name: 'command',
        message: 'Watchawannado',
        choices: ['Show state', 'Transfer money', 'Generate payment data']
      }
    ]
    const inquiryResult = await inquirer.prompt(questions)
    try {
      
    if (inquiryResult.command === 'Show state') {
      await showState()
    } else if (inquiryResult.command === 'Transfer money') {
      await transferMoney(
        web3, platformState, secretStore, k0Eth, k0, publicKeys
      )
    } else if (inquiryResult.command === 'Generate payment data') {
      await generatePaymentData(secretStore, k0)
    } else {
      throw new Error(`Unknown command: ${inquiryResult.command}`)
    }
    } catch(e) {
      console.log(e.stack)
      console.log('Error!', e.message)
    }
  }

  while (true) {
    await cycle()
  }

}

run().then(console.log).catch(console.log)
