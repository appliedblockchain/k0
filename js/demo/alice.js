'use strict'
const bip39 = require('bip39')
const hdkey = require('ethereumjs-wallet/hdkey')
const BN = require('bn.js')
const crypto = require('crypto')
const addresses = require('./addresses')
const testUtil = require('../test/util')
const makeEthPlatform = require('../eth')
const makeK0 = require('../k0')
const mnemonics = require('./mnemonics')
const signTransaction = require('../eth/sign-transaction')
const compileContracts = require('../test/helpers/compile-contracts')
const printState = require('./print-state')
const makeState = require('../state')
const { prompt } = require('./util')
const u = require('../util')

async function run() {
  const state = await makeState(parseInt(process.env.SERVER_PORT || '4000', 10))
  const web3 = testUtil.initWeb3()
  const k0Eth = await makeEthPlatform(
    web3,
    u.hex2buf(addresses.MVPPT)
  )
  const a_sk = crypto.randomBytes(32)
  const proverPort = parseInt(process.env.SERVER_PORT || '4000', 10)
  const k0 = await makeK0(a_sk, k0Eth, state, proverPort)

  const artefacts = await compileContracts()

  await prompt()

  console.log('alice go')
}

run().then(console.log).catch(console.log)
