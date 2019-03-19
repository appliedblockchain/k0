const makeK0 = require('../k0')
const bufferOfHex0x = require('@appliedblockchain/helpers/buffer-of-hex0x')
const addresses = require('./addresses')
const testUtil = require('../test/util')

async function run() {
  const k0 = await makeK0(
    testUtil.initWeb3(),
    bufferOfHex0x(addresses.MVPPT)
  )
}

run().then(console.log).catch(console.log)
