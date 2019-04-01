'use strict'

const makeFabric = require('../../fabric')
const u = require('../../util')

async function run() {
  const k0Fabric = await makeFabric(process.env.CHAINCODE_ID)
  const res = await k0Fabric.get()
  await k0Fabric.set(process.argv[2])
  await u.wait(10000)
}

run().then(console.log).catch(console.log)
