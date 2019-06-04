'use strict'

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
  const config = getConfig('bank', 'Admin', process.env.DEV_MODE === 'true')

  const bankAdmin = await makeClient(config)
  const bankPlatformState = await makePlatformState(config.mtServerPort)

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
})
