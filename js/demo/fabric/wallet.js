'use strict'

const _ = require('lodash')
const fs = require('fs')
const BN = require('bn.js')
const crypto = require('crypto')
const testUtil = require('../../test/util')
const makeFabricPlatform = require('../../fabric')
const makeK0 = require('../../k0')
const u = require('../../util')
const makePlatformState = require('../../platform-state')
const makeSecretStore = require('../../secret-store')
const log4js = require('log4js')
const demoUtil = require('../util')
const serverPorts = require('../server-ports')
const mtServerPorts = require('../mt-server-ports')
const initEventHandlers = require('./init-event-handlers')
const clear = require('clear')
const chalk = require('chalk')
const inquirer = require('inquirer')
const printState = require('./print-state')
const publicKeysInput = require('./public-keys')

const logger = log4js.getLogger()
logger.level = process.env.LOG_LEVEL || 'info'

process.on('unhandledRejection', error => {
  logger.error(error)
  process.exit(1)
})

const addressBookData = {}
addressBookData[publicKeysInput.alice] = 'Alice'
addressBookData[publicKeysInput.bob] = 'Bob'
addressBookData[publicKeysInput.carol] = 'Carol'

function addressBook(a_pk) {
  u.checkBuf(a_pk, 32)
  return addressBookData[u.buf2hex(a_pk)] || null
}


async function run() {

  const who = process.argv[2]
  if (['alice', 'bob', 'carol'].indexOf(who) === -1) {
    console.log('Need parameter "alice", "bob" or "carol".')
    process.exit(1)
  }
  const platformState = await makePlatformState(mtServerPorts[who])
  const k0Fabric = await makeFabricPlatform(process.env.CHAINCODE_ID)

  const secretStoreData = require(`./${who}.secrets.json`)
  const secretStore = makeSecretStore(secretStoreData)
  console.log('private key', secretStore.getPrivateKey())
  console.log('public key', secretStore.getPublicKey())
  initEventHandlers(platformState, secretStore, k0Fabric)

  const k0 = await makeK0(serverPorts[who])


  function showState() {
    console.log('state')
    return printState(secretStore, addressBook, k0Fabric, platformState)
  }

  await u.wait(1000)
  await k0Fabric.startEventMonitoring()

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
        // await transferMoney(
        //   web3, platformState, secretStore, k0Eth, k0, publicKeys
        // )
      } else {
        throw new Error(`Unknown command: ${inquiryResult.command}`)
      }
    } catch(e) {
      console.log(e.stack)
      console.log('Error!', e.message)
    }
    console.log()
  }

  await u.wait(1000)

  //clear()

  console.log(chalk.cyan([
    '',
    '██╗  ██╗ ██████╗',
    '██║ ██╔╝██╔═████╗',
    '█████╔╝ ██║██╔██║',
    '██╔═██╗ ████╔╝██║',
    '██║  ██╗╚██████╔╝',
    '╚═╝  ╚═╝ ╚═════╝',
    ''
  ].join('\n')))

  while (true) {
    await cycle()
  }

}

run().then(console.log).catch(console.log)
