const makeFabric = require('../fabric')

async function run() {
  const k0Fabric = await makeFabric(process.env.CHAINCODE_ID)
  const res = await k0Fabric.get()
  console.log(res.toString('utf8'))
  await k0Fabric.set('yo')
}

run().then(console.log).catch(console.log)
