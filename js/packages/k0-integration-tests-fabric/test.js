'use strict'

const BN = require('bn.js')
const _ = require('lodash')
const assert = require('assert')
const awaitEvent = require('./helpers/await-event')
const crypto = require('crypto')
const expect = require('code').expect
const getConfig = require('./helpers/get-config')
const log4js = require('log4js')
const makeEventHub = require('./event-hub')
const makeFabricPlatform = require('@appliedblockchain/k0-fabric')
const makeK0 = require('@appliedblockchain/k0')
const makePlatformState = require('@appliedblockchain/k0-in-memory-platform-state')
const makeSecretStore = require('@appliedblockchain/k0-in-memory-secret-store')
const testUtil = require('./helpers/util')
const u = require('@appliedblockchain/k0-util')
const url = require('url')
const waitPort = require('wait-port')

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

  before(async function () {
    logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'

    const devMode = u.readBooleanFromENV('DEV_MODE')


    process.stdout.write('Waiting for servers to become available...')
    let ready = false
    const serverWaitStart = Date.now()
    while (!ready) {
      try {
        const results = await Promise.all(orgs.map(o => {
          const config = getConfig(o, 'User1', devMode)
          const port = parseInt(url.parse(config.serverEndpoint).port, 10)
          return waitPort({ port })
        }))
        ready = results.reduce((a, b) => a & b, true)
      } catch (e) {
        console.log(e)
        process.stdout.write('.')
        await u.wait(1000)
      }
    }
    process.stdout.write('\n')
    console.log(`Waited: ${Date.now() - serverWaitStart}`)

    for (let i = 0; i < orgs.length; i = i + 1) {
      const who = orgs[i]
      const config = getConfig(who, 'User1', devMode)
      platformStates[who] = await makePlatformState(config.mtServerEndpoint)
      await platformStates[who].reset()
      k0s[who] = await makeK0(config.serverEndpoint)
      const privateKey = crypto.randomBytes(32)
      secretStores[who] = await initSecretStore(config.serverEndpoint, k0s[who], privateKey)
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

  function awaitMinting() {
    const prs = orgs.map(orgName => {
      return testUtil.awaitEvent(
        events[orgName],
        'mintProcessed',
        100
      )
    })

    return Promise.all(prs)
  }

  function txMinedOnNetwork(txId) {
    return Promise.all(orgs.map(orgName => {
      return k0Fabrics[orgName].waitForTx(txId).then(res =>{
        logger.debug(JSON.stringify({ label: 'MinedOnNetwork-SUCCESS', orgName, res }))
      }).catch(err => {
        logger.debug(JSON.stringify({ label: 'MinedOnNetwork-ERROR', orgName, err }))
        throw err
      })
    }))
  }

  async function printStates(message) {
    console.log(message)
    for (let i = 0; i < orgs.length; i++) {
      const orgName = orgs[i]
      const state = await k0Fabrics[orgName].getState()
      const platformStateRoot = await platformStates[orgName].merkleTreeRoot()
      console.log({
        label: `STATE for ${orgName}`,
        state: {
          root: state.root.toString('hex'),
          numLeaves: state.numLeaves.toString(),
          platformStateRoot: platformStateRoot.toString('hex')
        }
      })
    }
  }


  it('Can get the peers\' state; they each have the same root and 0 leaves', async function () {
    let root
    for (let i = 0; i < orgs.length; i++) {
      const orgName = orgs[i]
      const state = await k0Fabrics[orgName].getState()
      if (i === 0) {
        root = state.root
      } else {
        assert(state.root.equals(root))
      }
      expect(state.numLeaves.eq(new BN(0))).to.equal(true)
    }
  })

  it('Mint', async function () {
    let mintedNotes = 0
    // Nobody should have any money
    for (let i = 0; i < orgs.length; i = i + 1) {
      expect(secretStores[orgs[i]].getAvailableNotes().length).to.equal(0)
    }

    const numInitialHodlers = 2
    const numInitialNotesPerHodler = 2
    for (let i = 0; i < numInitialHodlers; i = i + 1) {
      logger.debug(`Minting notes for org ${i}`)
      const who = orgs[i]
      const values = _.times(numInitialNotesPerHodler, () => {
        return new BN(_.random(50).toString() + '000')
      })
      values.reduce((acc, el) => acc.add(el), new BN('0'))

      for (let j = 0; j < values.length; j++) {
        logger.debug(`Minting note ${j}`)
        const v = values[j]

        // In fabric, a single bank authority issues secret notes
        const data = await k0s[BANK].prepareDeposit(
          platformStates[BANK], secretStores[who].getAddress(), v
        )
        try {
          const mintProcessedPromises = awaitMinting()
          const { transactionId } = await k0Fabrics[BANK].mint( // eslint-disable-line
            data.k,
            v,
            data.cm,
            data.ciphertext,
            data.nextRoot,
            data.commitmentProofJacobian,
            data.additionProofJacobian
          )

          await Promise.all([
            awaitMinting,
            mintProcessedPromises,
            txMinedOnNetwork(transactionId)
          ])
          await u.wait(2000)
        } catch (err) {
          console.log('MINTING ERROR', err)
          await printStates('STATES ON ERROR')
          throw err
        }

        mintedNotes++
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

  it('Transfer', async function transferTest() {
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
