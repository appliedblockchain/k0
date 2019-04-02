const EventEmitter = require('events')
const makeClient = require('./client')
const get = require('./actions/get')
const set = require('./actions/set')
const mint = require('./actions/mint')
const u = require('../util')
const initEventHandlers = require('./init-event-handlers')

class K0Fabric extends EventEmitter {}

async function makeFabric(chaincodeId) {
  const { client, channel, peers, queryPeer } = await makeClient()

  const channelEventHub = channel.newChannelEventHub(queryPeer)

  const fabric = new K0Fabric()

  let regId1

  async function startEventMonitoring() {
    console.log('starting eventMonitoring')
    let queue = []
    let processing = false

    regId1 = initEventHandlers(channelEventHub, chaincodeId, fabric)
    channelEventHub.connect(true)
    await u.wait(1000)
    console.log('connected 1?', channelEventHub.isconnected())
    channelEventHub.checkConnection(true)
    await u.wait(1000)
    console.log('connected 2?', channelEventHub.isconnected())
  }
  function off() {
    channelEventHub.unregisterChaincodeEvent(regId1)
    channelEventHub.disconnect()
  }


  fabric.mint = mint.bind(null, client, channel, chaincodeId, peers)
  fabric.startEventMonitoring = startEventMonitoring
  fabric.off = off
  return fabric
}

module.exports = makeFabric
