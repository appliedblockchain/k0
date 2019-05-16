'use strict'

const BN = require('bn.js')
const Immutable = require('immutable')
const _ = require('lodash')
const bip39 = require('bip39')
const crypto = require('crypto')
const fs = require('fs')
const getConfig = require('./helpers/get-config')
const log4js = require('log4js')
const makeFabricPlatform = require('../../fabric')
const makeK0 = require('../../k0')
const makePlatformState = require('../../platform-state')
const makeSecretStore = require('../../secret-store')
const testUtil = require('../../test/util')
const u = require('../../util')

describe('Fabric workflow', function() {
  this.timeout(3600 * 1000)
  const privKeys = {},
        pubKeys = {}
  let logger

  it('Init', async function() {
    logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'

    const alphaConfig = getConfig('alpha', 'Admin')
    const platformState = await makePlatformState(alphaConfig.mtServerPort)
    const initialRoot = await platformState.merkleTreeRoot()

    privKeys.alpha = crypto.randomBytes(32)
    privKeys.beta = crypto.randomBytes(32)
    privKeys.gamma = crypto.randomBytes(32)

    const k0 = await makeK0(alphaConfig.proverPort)

    pubKeys.alpha = await k0.prfAddr(privKeys.alpha)
    pubKeys.beta = await k0.prfAddr(privKeys.beta)
    pubKeys.gamma = await k0.prfAddr(privKeys.gamma)
  })

  it('Mint', async function() {
    const initialHodlers = [ 'alpha', 'beta' ]
    for (let i = 0; i < 2; i = i + 1) {
      const who = initialHodlers[i]
      const config = getConfig(who, 'User1')
      const platformState = await makePlatformState(config.mtServerPort)
      const k0Fabric = await makeFabricPlatform(logger, config, 'k0chaincode')
      const privateKey = u.buf2hex(privKeys[who])
      const publicKey = u.buf2hex(pubKeys[who])
      const secretStoreData = Immutable.Map({
        privateKey,
        publicKey,
        cms: Immutable.Map()
    })
      const secretStore = makeSecretStore(secretStoreData)
      const values = _.times(3, () => new BN(_.random(50).toString() + '000'))
      const total = values.reduce((acc, el) => acc.add(el), new BN('0'))
      const k0 = await makeK0(config.proverPort)
      for (let i = 0; i < values.length; i++) {
        const v = values[i]
        const data = await k0.prepareDeposit(platformState, secretStore, v)
        await secretStore.addNoteInfo(data.cm, data.a_pk, data.rho, data.r, v)
        const depositTx = await k0Fabric.mint(
          u.buf2hex(data.cm),
          u.buf2hex(data.nextRoot),
          ''
        )
        await u.wait(200)
      }
    }
  })
})
