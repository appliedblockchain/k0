'use strict'

const endorsementPolicy = require('./endorsement-policy')
const getConfig = require('./helpers/get-config')
const instantiateOrUpgrade = require('./helpers/instantiate-or-upgrade')
const makeClient = require('./helpers/client')
const makePlatformState = require('@appliedblockchain/k0-in-memory-platform-state')
const u = require('@appliedblockchain/k0-util')

process.on('unhandledRejection', error => {
  console.log(error.message)
  console.log(error.stack)
  process.exit(1)
})

async function run() {

  const devMode = u.readBooleanFromENV('DEV_MODE')

  const config = getConfig('bank', 'Admin', devMode)

  const bankAdmin = await makeClient(config)
  const bankPlatformState = await makePlatformState(config.mtServerEndpoint)

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
