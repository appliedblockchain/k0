const makeK0 = require('@appliedblockchain/k0')

process.on('unhandledRejection', error => {
  console.error(error)
  process.exit(1)
})

async function run() {
  const k0 = await makeK0(process.env.K0_SERVER_ENDPOINT || 'http://localhost:4000')
  process.stdout.write('Waiting for server to become ready.')
  let k0ready = await k0.ready()
  while (!k0ready) {
    process.stdout.write('.')
    await u.wait(1000)
    k0ready = await k0.ready()
  }
  process.stdout.write('\nServer ready.\n')
}

run()
