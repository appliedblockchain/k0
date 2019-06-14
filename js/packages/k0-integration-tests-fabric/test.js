'use strict'

const BN = require('bn.js')
const _ = require('lodash')
const assert = require('assert')
const awaitEvent = require('./helpers/await-event')
const crypto = require('crypto')
const getConfig = require('./helpers/get-config')
const log4js = require('log4js')
const makeFabricPlatform = require('@appliedblockchain/k0-fabric')
const makeK0 = require('@appliedblockchain/k0')
const makePlatformState = require('@appliedblockchain/k0-in-memory-platform-state')
const makeSecretStore = require('@appliedblockchain/k0-in-memory-secret-store')
const u = require('@appliedblockchain/k0-util')
const expect = require('code').expect
const makeEventHub = require('./event-hub')

async function initSecretStore(port, k0, secretKey) {
  const { a_pk, sk_enc, pk_enc } =
        await k0.deriveKeys(secretKey)
  return makeSecretStore(secretKey, a_pk, sk_enc, pk_enc)
}

describe('Fabric workflow', function fabricTest() {
  this.timeout(3600 * 1000)
  const k0s = {}
  const platformStates = {}
  const secretStores = {}
  const k0Fabrics = {}
  // event handling
  const events = {}
  const addresses = {}
  const orgs = [ 'alpha', 'beta', 'gamma', 'bank' ]
  const BANK = 'bank'
  let logger

  it('Init', async function () {
    logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'
    for (let i = 0; i < orgs.length; i = i + 1) {
      const who = orgs[i]
      const config = getConfig(who, 'User1', process.env.DEV_MODE === 'true')
      platformStates[who] = await makePlatformState(config.mtServerPort)
      await platformStates[who].reset()
      k0s[who] = await makeK0(config.proverPort)
      const privateKey = crypto.randomBytes(32)
      secretStores[who] = await initSecretStore(config.proverPort, k0s[who], privateKey)
      addresses[who] = secretStores[who].getAddress()
      k0Fabrics[who] = await makeFabricPlatform(
        logger,
        config,
        process.env.CHAINCODE_ID || 'k0chaincode'
      )
      events[who] = makeEventHub(
        platformStates[who],
        secretStores[who],
        k0s[who],
        k0Fabrics[who]
      )
      k0Fabrics[who].startEventMonitoring()
    }

    // Wait for the event queue to initialise
    await u.wait(2000)

    // TODO The following is still a bit fragile. A better way would be to wait
    // until the platform state Merkle tree root matches that of the actual
    // platform (once the chaincode provides functionality to query the root).

    // If not all event queues are empty
    if (!orgs.map(who => events[who].queueEmpty()).reduce((a, b) => a && b)) {
      // wait for all event queues to become empty
      await Promise.all(orgs.map(who => {
        return awaitEvent(events[who], 'queueEmpty', 100)
      }))
    }
  })

  it('Mint', async function () {
    // Nobody should have any money
    for (let i = 0; i < orgs.length; i = i + 1) {
      expect(secretStores[orgs[i]].getAvailableNotes().length).to.equal(0)
    }
    const numInitialHodlers = 2
    const numInitialNotesPerHodler = 2
    for (let i = 0; i < numInitialHodlers; i = i + 1) {
      logger.info(`Minting Notes for Org ${i}`)
      const who = orgs[i]
      const values = _.times(numInitialNotesPerHodler, () => {
        return new BN(_.random(50).toString() + '000')
      })
      values.reduce((acc, el) => acc.add(el), new BN('0'))

      for (let j = 0; j < values.length; j++) {
        logger.info(`Minting note ${j}`)
        const v = values[j]

        // In fabric, a single bank authority issues secret notes
        const data = await k0s[BANK].prepareDeposit(
          platformStates[BANK], secretStores[who].getAddress(), v
        )
        const mintProcessedPromise = awaitEvent(
          events[BANK],
          'mintProcessed',
          100
        )
        const depositTx = await k0Fabrics[BANK].mint( // eslint-disable-line
          data.k,
          v,
          data.cm,
          data.ciphertext,
          data.nextRoot,
          data.commitmentProofJacobian,
          data.additionProofJacobian
        )
        await mintProcessedPromise
        if (process.env.CIRCLECI) {
          await u.wait(1000)
        }
      }
      logger.info(`Note for org ${i} MINTED`)
    }

    // Hodlers should now have numInitialNotesPerHodler notes each
    for (let i = 0; i < numInitialHodlers; i = i + 1) {
      const numAvailableNotes = secretStores[orgs[i]].getAvailableNotes().length
      expect(numAvailableNotes).to.equal(numInitialNotesPerHodler)
    }
    // GammaCo should still have 0
    expect(secretStores[orgs[2]].getAvailableNotes().length).to.equal(0)

  })

  it('Transfer', async function transferTest() {

    const labelBeforeBefore = platformStates.alpha.currentStateLabel()

    const availableNotes = secretStores.alpha.getAvailableNotes()
    // select 2 notes randomly
    const inputs = _.sampleSize(availableNotes, 2)
    const sum = _.map(inputs, 'v').reduce((acc, el) => acc.add(el), new BN(0))
    const thousand = new BN(1000)
    const in0addr = platformStates.alpha.indexOfCM(inputs[0].cm)
    expect(in0addr).to.not.be.null()
    const in1addr = platformStates.alpha.indexOfCM(inputs[1].cm)
    expect(in1addr).to.not.be.null()

    const out0 = {
      address: addresses.gamma,
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48),
      v: thousand.mul(new BN(_.random(sum.div(thousand).toNumber())))
    }

    const out1 = {
      address: addresses.alpha,
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

    // This is hacky :/
    // We need a "simulate two additions" function on the MT server
    const rootBefore = await platformStates.alpha.merkleTreeRoot()
    const labelBefore = platformStates.alpha.currentStateLabel()
    const tmpLabel =
      `temporary_mt_addition_${crypto.randomBytes(16).toString('hex')}`
    await platformStates.alpha.add(
      tmpLabel, [], [ transferData.output_0_cm, transferData.output_1_cm ]
    )
    const newRoot = await platformStates.alpha.merkleTreeRoot()
    await platformStates.alpha.rollbackTo(labelBefore)

    const finalRoot = await platformStates.alpha.merkleTreeRoot()
    assert(finalRoot.equals(rootBefore))

    await k0Fabrics.alpha.transfer(
      transferData.input_0_sn,
      transferData.input_1_sn,
      transferData.output_0_cm,
      transferData.output_1_cm,
      transferData.output_0_ciphertext,
      transferData.output_1_ciphertext,
      newRoot
    )
  })

  after(async function () {
    for (let i = 0; i < orgs.length; i = i + 1) {
      k0Fabrics[orgs[i]].off()
    }
  })
})
