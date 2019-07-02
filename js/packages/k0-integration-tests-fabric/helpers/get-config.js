'use-strict'

const fs = require('fs')
const getDevModePeers = require('./get-dev-mode-peers')
const getPeers = require('./get-peers')
const path = require('path')
const readFile = require('./read-file')

const firstFileInDir = dir => path.join(dir, fs.readdirSync(dir)[0])

const orgs = [ 'alpha', 'beta', 'gamma', 'bank' ]
const usernames = [ 'Admin', 'User1' ]

function getConfig(org, username, devMode = false) {
  const orgIndex = orgs.indexOf(org)
  if (orgIndex === -1) {
    throw new Error(`Invalid org: ${org}`)
  }
  if (usernames.indexOf(username) === -1) {
    throw new Error(`Invalid username: ${username}`)
  }
  const orgNum = orgIndex + 1
  const config = {
    username,
    mspid: org.charAt(0).toUpperCase() + org.slice(1) + 'CoMSP',
    queryPeerIndex: orgs.indexOf(org),
    channelName: 'the-channel',
    serverEndpoint: `http://localhost:4000`,
    mtServerEndpoint: `http://localhost:${10000 + orgNum * 1000 + 410}`
  }
  if (devMode) {
    Object.assign(config, {
      userCertPEM: readFile(firstFileInDir(`${__dirname}/../devnetwork/crypto-config/peerOrganizations/${org}.com/users/${username}@${org}.com/msp/signcerts`)),
      userPrivKeyPEM: readFile(firstFileInDir(`${__dirname}/../devnetwork/crypto-config/peerOrganizations/${org}.com/users/${username}@${org}.com/msp/keystore`)),
      ordererUrl: 'grpc://localhost:7050',
      peers: getDevModePeers()
    })
  } else {
    Object.assign(config, {
      userCertPEM: readFile(firstFileInDir(`${__dirname}/../network/crypto-config/peerOrganizations/${org}.com/users/${username}@${org}.com/msp/signcerts`)),
      userPrivKeyPEM: readFile(firstFileInDir(`${__dirname}/../network/crypto-config/peerOrganizations/${org}.com/users/${username}@${org}.com/msp/keystore`)),
      ordererUrl: 'grpcs://localhost:7050',
      ordererTlsCaCertPEM: readFile(`${__dirname}/../network/crypto-config/ordererOrganizations/orderer.org/tlsca/tlsca.orderer.org-cert.pem`),
      ordererTlsHostnameOverride: 'orderer.orderer.org',
      peers: getPeers()
    })
  }
  return config
}

module.exports = getConfig
