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
const serverPorts = require('./server-ports')
const mtServerPorts = require('./mt-server-ports')
const initEventHandlers = require('./init-event-handlers')

const logger = log4js.getLogger()
logger.level = process.env.LOG_LEVEL || 'info'

process.on('unhandledRejection', error => {
  logger.error(error)
  process.exit(1)
})

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

  const values = _.times(3, () => new BN(_.random(50).toString() + '000'))

  const total = values.reduce((acc, el) => acc.add(el), new BN('0'))

  await demoUtil.prompt()

  for (let i = 0; i < values.length; i++) {
    const v = values[i]

    const data = await k0.prepareDeposit(platformState, secretStore, v)
    await secretStore.addNoteInfo(data.cm, data.a_pk, data.rho, data.r, v)

    const depositTx = await k0Fabric.mint(
      u.buf2hex(data.cm),
      u.buf2hex(data.nextRoot),
      ''
    )

    await u.wait(200)
  }

  fs.writeFileSync(`${who}.secrets.json`, JSON.stringify(secretStore.spit()))

}

run().then(console.log).catch(console.log)
