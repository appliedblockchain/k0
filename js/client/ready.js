const util = require('../util')

async function ready(jc) {
  let ready = false;

  process.stdout.write('Waiting for the server to become ready...')
  while (!ready) {
    const statusResponse = await jc.request('status', [])
    ready = statusResponse.result.ready
    if (!ready) {
      process.stdout.write('.')
      await util.wait(1000)
    }
  }
  process.stdout.write('\n')
  console.log('Server ready.')
}

module.exports = ready
