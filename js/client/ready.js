const util = require('../util')

async function ready(jc, serverName = '') {
  let ready = false

  // process.stdout.write(`Waiting for the ${serverName} server to become ready...`)
  while (!ready) {
    const statusResponse = await jc.request('status', [])
    ready = statusResponse.result.ready

    if (!ready) {
      process.stdout.write('.')
      await util.wait(1000)
    }
  }
  process.stdout.write('\n')
  console.log(`${serverName} server ready.`)
}

module.exports = ready
