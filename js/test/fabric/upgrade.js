'use strict'

const getConfig = require('./helpers/get-config')
const makeClient = require('./helpers/client')
const instantiateOrUpgrade = require('./helpers/instantiate-or-upgrade')
const endorsementPolicy = require('./endorsement-policy')

process.on('unhandledRejection', error => {
  console.log(error.message)
  console.log(error.stack)
  process.exit(1)
})

async function run() {
  const config = getConfig('alpha', 'Admin')
  const alphaAdmin = await makeClient(config)
  await instantiateOrUpgrade(
    alphaAdmin,
    endorsementPolicy,
    'k0chaincode',
    process.env.CHAINCODE_VERSION || '1',
    [ 'bar' ],
    true
  )
}

run()
