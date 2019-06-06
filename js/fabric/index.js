const EventEmitter = require('events')
const initEventHandlers = require('./init-event-handlers')
const makeClient = require('./client')
const mint = require('./actions/mint')
const notalogger = require('@appliedblockchain/not-a-logger')
const transfer = require('./actions/transfer')
const u = require('../util')

class K0Fabric extends EventEmitter {}

async function makeFabric(logger, config, chaincodeId) {
  logger = logger || notalogger
  const { client, channel, peers, queryPeer } = await makeClient(config)

  const eh = channel.newChannelEventHub(queryPeer)

  const fabric = new K0Fabric()

  let regId1

  async function startEventMonitoring() {
    regId1 = initEventHandlers(eh, chaincodeId, fabric)
    eh.connect(true)
    await u.wait(1000)
  }

  function off() {
    eh.unregisterChaincodeEvent(regId1)
    eh.disconnect()
  }


  fabric.mint = mint.bind(null, logger, client, channel, chaincodeId, peers,
    queryPeer)
  fabric.transfer = transfer.bind(null, logger, client, channel, chaincodeId,
    peers, queryPeer)
  fabric.startEventMonitoring = startEventMonitoring
  fabric.off = off
  return fabric
}

module.exports = makeFabric
