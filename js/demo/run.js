'use strict'
const BN = require('bn.js')
const crypto = require('crypto')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const u = require('../util')

async function run() {
  const k0Eth = await makeEthPlatform(
    testUtil.initWeb3(),
    u.hex2buf(addresses.MVPPT)
  )
  const a_sk = crypto.randomBytes(32)
  const k0 = await makeK0(a_sk)
  const data = await k0.prepareDeposit(new BN('50000'))
 }

run().then(console.log).catch(console.log)