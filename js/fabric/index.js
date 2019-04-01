const makeClient = require('./client')
const get = require('./actions/get')
const set = require('./actions/set')
const u = require('../util')

async function makeFabric(chaincodeId) {
  const { client, channel, peers, queryPeer } = await makeClient()
  console.log(queryPeer)

  const channelEventHub = channel.newChannelEventHub(queryPeer)

  channelEventHub.connect(true)

  const regId1 = channelEventHub.registerChaincodeEvent(
    chaincodeId,
    '^.*',
    (event, block_num, txnid, status) => {
      console.log('Successfully got a chaincode event with transid:'+ txnid + ' with status:'+status);
      console.log({ event, block_num, txnid, status })

      console.log('connected?', channelEventHub.isconnected())
      // to see the event payload, the channel_event_hub must be connected(true)
      let event_payload = event.payload.toString('utf8');
      console.log({ event_payload })
    })

  await u.wait(1000)
  console.log('connected?', channelEventHub.isconnected())
  channelEventHub.checkConnection(true)
  await u.wait(1000)
  console.log('connected?', channelEventHub.isconnected())


  return {
    get: get.bind(null, channel, chaincodeId, queryPeer),
    set: set.bind(null, client, channel, chaincodeId, peers)
  }
}

module.exports = makeFabric
