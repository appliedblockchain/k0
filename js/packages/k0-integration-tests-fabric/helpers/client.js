'use strict'
const Client = require('fabric-client')
const fs = require('fs')

const userConfig = config => {
  return {
    username: config.username,
    mspid: config.mspid,
    cryptoContent: {
      privateKeyPEM: config.userPrivKeyPEM,
      signedCertPEM: config.userCertPEM
    }
  }
}

const dummyKeyValueStore = {
  getValue: (name) => null,
  setValue: (name, value) => Promise.resolve()
}

class DummyKeyValueStore {
  constructor() {
    return Promise.resolve(this)
  }
  async getValue() {
    return null
  }
  async setValue(name, value) {
  }

}

async function makeClient(config) {
  const client = new Client()
  client.setStateStore(dummyKeyValueStore)
  const cryptoSuite = Client.newCryptoSuite()
  cryptoSuite.setCryptoKeyStore(Client.newCryptoKeyStore(DummyKeyValueStore, {}))
  client.setCryptoSuite(cryptoSuite)

  const channel = client.newChannel(config.channelName)

  const ordererOptions = {
    'pem': config.ordererTlsCaCertPEM
  }
  if (config.ordererTlsHostnameOverride !== undefined
      && config.ordererTlsHostnameOverride.length > 0) {
    ordererOptions['ssl-target-name-override'] = config.ordererTlsHostnameOverride
  }
  const orderer = client.newOrderer(config.ordererUrl, ordererOptions)
  channel.addOrderer(orderer)

  const user = await client.createUser(userConfig(config))

  const peers = config.peers.map(p => {
    const peerOptions = {
      'pem': p.tlsCaCertPEM,
      'request-timeout': 3000000
    }
    if (p.tlsHostnameOverride !== undefined
        && p.tlsHostnameOverride.length > 0) {
		  peerOptions['ssl-target-name-override'] = p.tlsHostnameOverride
    }
    return client.newPeer(p.url, peerOptions)
  })

  const queryPeer = peers[config.queryPeerIndex]

  return { client, channel, peers, queryPeer }
}

module.exports = makeClient
