'use-strict'

const readFile = require('./read-file')
const getPeers = require('./get-peers')
const path = require('path')
const fs = require('fs')

const firstFileInDir = dir => path.join(dir, fs.readdirSync(dir)[0])

const orgs = [ 'alpha', 'beta', 'gamma', 'bank' ]
const usernames = [ 'Admin', 'User1' ]

function getConfig(org, username) {
  const orgIndex = orgs.indexOf(org)
  if (orgIndex === -1) {
    throw new Error(`Invalid org: ${org}`)
  }
  if (usernames.indexOf(username) === -1) {
    throw new Error(`Invalid username: ${username}`)
  }
  const orgNum = orgIndex + 1

  const cryptoConfigDir = `${__dirname}/../network/crypto-config`
  return {
    username,
    mspid: org.charAt(0).toUpperCase() + org.slice(1) + 'CoMSP',
    userCertPEM: readFile(firstFileInDir(`${cryptoConfigDir}/peerOrganizations/${org}.com/users/${username}@${org}.com/msp/signcerts`)),
    userPrivKeyPEM: readFile(firstFileInDir(`${cryptoConfigDir}//peerOrganizations/${org}.com/users/${username}@${org}.com/msp/keystore`)),
    ordererUrl: 'grpcs://localhost:7050',
    ordererTlsCaCertPEM: readFile(`${cryptoConfigDir}/ordererOrganizations/orderer.org/tlsca/tlsca.orderer.org-cert.pem`),
    ordererTlsHostnameOverride: 'orderer.orderer.org',
    queryPeerIndex: orgs.indexOf(org),
    channelName: 'the-channel',
    peers: getPeers(),
    proverPort: 10000 + (orgNum * 1000) + 400,
    mtServerPort: 10000 + (orgNum * 1000) + 410
  }
}

module.exports = getConfig
