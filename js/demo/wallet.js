'use strict'

const bip39 = require('bip39')
const hdkey = require('ethereumjs-wallet/hdkey')
const BN = require('bn.js')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const mnemonics = require('./mnemonics')
const compileContracts = require('../test/helpers/compile-contracts')
const printState = require('./print-state')
const makePlatformState = require('../platform-state')
const makeSecretStore = require('../secret-store')
const u = require('../util')
const { hex2buf } = require('../util')
const inquirer = require('inquirer')
const publicKeysInput = require('./public-keys')
const transferMoney = require('./transfer-money')
const generatePaymentData = require('./generate-payment-data')
const deployTradingContract = require('./deploy-trading-contract')
const initEventHandlers = require('./init-event-handlers')
const serverPorts = require('./server-ports')
const mtServerPorts = require('./mt-server-ports')
const chalk = require('chalk')
const clear = require('clear')

process.on('unhandledRejection', error => {
  console.log(error)
  process.exit(1)
})

const publicKeys = {}
Object.keys(publicKeysInput).forEach(name => {
  publicKeys[name] = u.hex2buf(publicKeysInput[name])
})

const addressBookData = {}
addressBookData[addresses.alice] = 'Alice'
addressBookData[addresses.bob] = 'Bob'
addressBookData[addresses.carol] = 'Carol'

function addressBook(address) {
  u.checkBuf(address, 20)
  return addressBookData[u.buf2hex(address)] || null
}

const mvpptAddressBookData = {}
mvpptAddressBookData[publicKeysInput.alice] = 'Alice'
mvpptAddressBookData[publicKeysInput.bob] = 'Bob'
mvpptAddressBookData[publicKeysInput.carol] = 'Carol'

function mvpptAddressBook(a_pk) {
  u.checkBuf(a_pk, 32)
  return mvpptAddressBookData[u.buf2hex(a_pk)] || null
}

async function run() {
  const who = process.argv[2]
  if (['alice', 'bob', 'carol'].indexOf(who) === -1) {
    console.log('Need parameter "alice", "bob" or "carol".')
    process.exit(1)
  }
  const platformState = await makePlatformState(mtServerPorts[who])
  const web3 = testUtil.initWeb3()
  const k0Eth = await makeEthPlatform(web3, u.hex2buf(addresses.MVPPT))

  const secretStoreData = require(`./${who}.secrets.json`)

  secretStoreData.cms = secretStoreData.cms || {}
  const noteInfos = Object.values(secretStoreData.cms).map((v, k) => {
    return {
      cm: hex2buf(Object.keys(secretStoreData.cms)[k]),
      a_pk: hex2buf(v.a_pk),
      rho: hex2buf(v.rho),
      r: hex2buf(v.r),
      v: new BN(v.v)
    }
  })

  const secretStore = makeSecretStore(
    u.hex2buf(secretStoreData.privateKey),
    u.hex2buf(secretStoreData.publicKey),
    noteInfos
  )
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
  const ethWallet = root.derivePath(path).getWallet()

  const carToken = new web3.eth.Contract(
    artefacts.CarToken.abi,
    addresses.CarToken
  )

  const carIds = [new BN('1')]
  function showState() {
    return printState(
      secretStore,
      addressBook,
      mvpptAddressBook,
      k0Eth,
      platformState,
      carToken,
      carIds
    )
  }

  async function cycle() {
    const questions = [
      {
        type: 'list',
        name: 'command',
        message: 'What do you want to do?',
        choices: [
          'Show state',
          'Transfer money',
          'Smart payment',
          'Generate payment data',
          'Deploy car trading smart contract'
        ]
      }
    ]
    const inquiryResult = await inquirer.prompt(questions)
    try {
      if (inquiryResult.command === 'Show state') {
        await showState()
      } else if (inquiryResult.command === 'Transfer money') {
        await transferMoney(
          web3,
          platformState,
          secretStore,
          k0Eth,
          k0,
          publicKeys
        )
      } else if (inquiryResult.command === 'Smart payment') {
        await transferMoney(
          web3,
          platformState,
          secretStore,
          k0Eth,
          k0,
          publicKeys,
          true,
          ethWallet.getPrivateKey()
        )
      } else if (inquiryResult.command === 'Generate payment data') {
        await generatePaymentData(secretStore, k0)
      } else if (
        inquiryResult.command === 'Deploy car trading smart contract'
      ) {
        await deployTradingContract(
          web3,
          artefacts,
          secretStore,
          ethWallet.getPrivateKey(),
          k0,
          carToken,
          u.hex2buf(addresses.MVPPT)
        )
      } else {
        throw new Error(`Unknown command: ${inquiryResult.command}`)
      }
    } catch (e) {
      console.log(e.stack)
      console.log('Error!', e.message)
    }
    console.log()
  }

  await u.wait(1000)

  clear()

  console.log(
    chalk.cyan(
      [
        '',
        '██╗  ██╗ ██████╗',
        '██║ ██╔╝██╔═████╗',
        '█████╔╝ ██║██╔██║',
        '██╔═██╗ ████╔╝██║',
        '██║  ██╗╚██████╔╝',
        '╚═╝  ╚═╝ ╚═════╝',
        ''
      ].join('\n')
    )
  )

  while (true) {
    await cycle()
  }
}

run()
  .then(console.log)
  .catch(console.log)
