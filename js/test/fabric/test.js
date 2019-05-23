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
const expect = require('code').expect
const initEventHandlers = require('./init-event-handlers')

async function generateSecretStore(k0) {
  const privateKey = crypto.randomBytes(32)
  const publicKey = await k0.prfAddr(privateKey)
  return makeSecretStore(privateKey, publicKey, [])
}

function makeData(a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  return Buffer.concat([ a_pk, rho, r, v.toBuffer('le', 64)])
}

describe('Fabric workflow', function() {
  this.timeout(3600 * 1000)
  const k0s = {}
  const platformStates = {}
  const secretStores = {}
  const k0Fabrics = {}
  const publicKeys = {}
  const orgs = [ 'alpha', 'beta', 'gamma' ]
  let logger

  it('Init', async function() {
    logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'
    for (let i = 0; i < orgs.length; i = i + 1) {
      const who = orgs[i]
      const config = getConfig(who, 'User1')
      platformStates[who] = await makePlatformState(config.mtServerPort)
      await platformStates[who].reset()
      k0s[who] = await makeK0(config.proverPort)
      secretStores[who] = await generateSecretStore(k0s[who])
      publicKeys[who] = secretStores[who].getPublicKey()
      k0Fabrics[who] = await makeFabricPlatform(
        logger,
        config,
        process.env.CHAINCODE_ID || 'k0chaincode'
      )
      initEventHandlers(platformStates[who], secretStores[who], k0Fabrics[who])
      k0Fabrics[who].startEventMonitoring()
    }
    await u.wait(5000)
    // TODO instead: wait until Fabric Merkle tree root equals platform state Merkle root
  })

  it('Mint', async function() {
    // Nobody should have any money
    for (let i = 0; i < orgs.length; i = i + 1) {
      expect(secretStores[orgs[i]].getAvailableNotes().length).to.equal(0)
    }
    const numInitialHodlers = 1
    const numInitialNotesPerHodler = 2
    for (let i = 0; i < numInitialHodlers; i = i + 1) {
      const who = orgs[i]
      const values = _.times(numInitialNotesPerHodler, () => {
        return new BN(_.random(50).toString() + '000')
      })
      const total = values.reduce((acc, el) => acc.add(el), new BN('0'))
      for (let i = 0; i < values.length; i++) {
        const v = values[i]
        const data = await k0s[who].prepareDeposit(
          platformStates[who], secretStores[who], v
        )
        console.log({ data })
        await secretStores[who].addNoteInfo(
          data.cm, data.a_pk, data.rho, data.r, v
        )
        const depositTx = await k0Fabrics[who].mint(
          u.buf2hex(data.cm),
          u.buf2hex(data.nextRoot),
          ''
        )
        await u.wait(2000)
      }
    }

    // Hodlers should now have numInitialNotesPerHodler notes each
    for (let i = 0; i < numInitialHodlers; i = i + 1) {
      const numAvailableNotes = secretStores[orgs[i]].getAvailableNotes().length
      expect(numAvailableNotes).to.equal(numInitialNotesPerHodler)
    }
    // GammaCo should still have 0
    expect(secretStores[orgs[2]].getAvailableNotes().length).to.equal(0)

  })

  it('Transfer', async function() {

    const labelBeforeBefore = platformStates.alpha.currentStateLabel()

    const availableNotes = secretStores.alpha.getAvailableNotes()
    // select 2 notes randomly
    const inputs = _.sampleSize(availableNotes, 2)
    const sum = _.map(inputs, 'v').reduce((acc, el) => acc.add(el), new BN(0))
    const thousand = new BN(1000)
    const in0addr = platformStates.alpha.indexOfCM(inputs[0].cm)
    const in1addr = platformStates.alpha.indexOfCM(inputs[1].cm)

    const out0 = {
      a_pk: publicKeys.gamma,
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48),
      v: thousand.mul(new BN(_.random(sum.div(thousand).toNumber())))
    }

    const out1 = {
      a_pk: publicKeys.alpha,
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48),
      v: sum.sub(out0.v)
    }

    const transferData = await k0s.alpha.prepareTransfer(
      platformStates.alpha,
      secretStores.alpha,
      in0addr,
      in1addr,
      out0,
      out1
    )

    secretStores.alpha.addSNToNote(inputs[0].cm, transferData.input_0_sn)
    secretStores.alpha.addSNToNote(inputs[1].cm, transferData.input_1_sn)
    secretStores.alpha.addNoteInfo(
      transferData.output_0_cm, out0.a_pk, out0.rho, out0.r, out0.v
    )
    secretStores.alpha.addNoteInfo(
      transferData.output_1_cm, out1.a_pk, out1.rho, out1.r, out1.v
    )

    // This is hacky :/
    // We need a "simulate two additions" function on the MT server
    const rootBefore = await platformStates.alpha.merkleTreeRoot()
    const labelBefore = platformStates.alpha.currentStateLabel()
    const tmpLabel = 'temporary_mt_addition_' + crypto.randomBytes(16).toString('hex')
    await platformStates.alpha.add(
      tmpLabel, [], [ transferData.output_0_cm, transferData.output_1_cm ]
    )
    const newRoot = await platformStates.alpha.merkleTreeRoot()
    await platformStates.alpha.rollbackTo(labelBefore)

    const finalRoot = await platformStates.alpha.merkleTreeRoot()
    assert(finalRoot.equals(rootBefore))

    const out_0_data = makeData(out0.a_pk, out0.rho, out0.r, out0.v)
    const out_1_data = makeData(out1.a_pk, out1.rho, out1.r, out1.v)

    await k0Fabrics.alpha.transfer(
      transferData.input_0_sn,
      transferData.input_1_sn,
      transferData.output_0_cm,
      transferData.output_1_cm,
      out_0_data,
      out_1_data,
      newRoot
    )
  })

  after(async function() {
    console.log('shutting down')
    for (let i = 0; i < orgs.length; i = i + 1) {
      k0Fabrics[orgs[i]].off()
    }
  })
})
