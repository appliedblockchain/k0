'use strict'

const bip39 = require('bip39')
const crypto = require('crypto')
const fs = require('fs')
const testUtil = require('../../test/util')
const _ = require('lodash')
const makePlatformState = require('../../platform-state')
const u = require('../../util')
const makeK0 = require('../../k0')
const log4js = require('log4js')

const logger = log4js.getLogger()
logger.level = process.env.LOG_LEVEL || 'info'

process.on('unhandledRejection', error => {
  logger.error(error)
  process.exit(1)
})

async function run() {

  const platformState = await makePlatformState()
  const initialRoot = await platformState.merkleTreeRoot()

  const aliceSecretKey = crypto.randomBytes(32)
  const bobSecretKey = crypto.randomBytes(32)
  const carolSecretKey = crypto.randomBytes(32)

  const k0 = await makeK0()
  const alicePublicKey = await k0.prfAddr(aliceSecretKey)
  const bobPublicKey = await k0.prfAddr(bobSecretKey)
  const carolPublicKey = await k0.prfAddr(carolSecretKey)
  fs.writeFileSync('public-keys.json', JSON.stringify({
    alice: u.buf2hex(alicePublicKey),
    bob: u.buf2hex(bobPublicKey),
    carol: u.buf2hex(carolPublicKey)
  }))
  fs.writeFileSync('alice.secrets.json', JSON.stringify({
    privateKey: u.buf2hex(aliceSecretKey),
    publicKey: u.buf2hex(alicePublicKey)
  }))
  fs.writeFileSync('bob.secrets.json', JSON.stringify({
    privateKey: u.buf2hex(bobSecretKey),
    publicKey: u.buf2hex(bobPublicKey)
  }))
  fs.writeFileSync('carol.secrets.json', JSON.stringify({
    privateKey: u.buf2hex(carolSecretKey),
    publicKey: u.buf2hex(carolPublicKey)
  }))

  logger.info([
    `Alice: public key ${u.buf2hex(alicePublicKey)}, `,
    `private key ${u.buf2hex(aliceSecretKey)}`
  ].join(''))
  logger.info([
    `Bob: public key ${u.buf2hex(bobPublicKey)}, `,
    `private key ${u.buf2hex(bobSecretKey)}`
  ].join(''))
  logger.info([
    `Carol: public key ${u.buf2hex(carolPublicKey)}, `,
    `private key ${u.buf2hex(carolSecretKey)}`
  ].join(''))
}

run().then(console.log).catch(console.log)
