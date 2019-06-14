'use strict'

const u = require('../../util')
const getConfig = require('./helpers/get-config')
const makeClient = require('./helpers/client')
const instantiateOrUpgrade = require('./helpers/instantiate-or-upgrade')
const endorsementPolicy = require('./endorsement-policy')
const makePlatformState = require('../../platform-state')

process.on('unhandledRejection', error => {
  console.log(error.message)
  console.log(error.stack)
  process.exit(1)
})

async function run() {

  const devMode = u.readBooleanFromENV('DEV_MODE')

  console.inspect({ devMode })

  const config = getConfig('bank', 'Admin', devMode)

  const bankAdmin = await makeClient(config)
  const bankPlatformState = await makePlatformState(config.mtServerPort)

  await bankPlatformState.reset()

  const initialRoot = await bankPlatformState.merkleTreeRoot()

  await instantiateOrUpgrade(
    bankAdmin,
    endorsementPolicy,
    process.env.CHAINCODE_ID || 'k0chaincode',
    process.env.CHAINCODE_VERSION || '1',
    [ initialRoot ],
    false
  )
}

run().then(() => {
  console.log('Chaincode successfuly instantiated.')
  process.exit(0)
})
