'use strict'
const _ = require('lodash')
const csvLoader = require('csv-load-sync')
const fs = require('fs')
const path = require('path')

const firstFileInDir = dir => path.join(dir, fs.readdirSync(dir)[0])

const read = filePath => Buffer.from(fs.readFileSync(filePath)).toString()

function generateConfig(config) {
  const peers = csvLoader(config.peersConfigFilePath).map(p => _.extend(
    _.pick(p, [ 'url', 'eventUrl', 'tlsHostnameOverride' ]),
    { tlsCaCertPEM: read(p.tlsCaCertPath) }
  ))
  const fieldsToBeCopied = [
    'chaincodeId',
    'channelName',
    'mspid',
    'ordererTlsHostnameOverride',
    'ordererUrl',
    'queryPeerIndex',
    'username'
  ]
  return _.extend(
    _.pick(config, fieldsToBeCopied),
    {
      ordererTlsCaCertPEM: read(config.ordererTlsCaCertPath),
      peers: peers,
      userCertPEM: read(firstFileInDir(config.userCertDir)),
      userPrivKeyPEM: read(firstFileInDir(config.userPrivKeyDir))
    }
  )
}

function getConfig() {
  const requiredEnvVars = [
    'CHAINCODE_ID',
    'CHANNEL_NAME',
    'MSPID',
    'ORDERER_TLS_CA_CERT_PATH',
    'ORDERER_URL',
    'ORG_LABEL',
    'PEERS_CONFIG_FILE_PATH',
    'USER_CERT_DIR',
    'USER_PRIVKEY_DIR'
  ]

  const missingEnvVars = requiredEnvVars.filter(variable => process.env[variable] === undefined)

  if (missingEnvVars.length > 0) {
    throw new Error(`Missing env vars: ${missingEnvVars.join(', ')}`)
  }

  console.log('Starting with the following configuration:')

  requiredEnvVars.forEach(envVar => {
    console.log(envVar, process.env[envVar])
  })

  return generateConfig({
    username: process.env['USERNAME'] || 'User1',
    mspid: process.env['MSPID'],
    userCertDir: process.env['USER_CERT_DIR'],
    userPrivKeyDir: process.env['USER_PRIVKEY_DIR'],
    ordererUrl: process.env['ORDERER_URL'],
    ordererTlsCaCertPath: process.env['ORDERER_TLS_CA_CERT_PATH'],
    ordererTlsHostnameOverride: process.env['ORDERER_TLS_HOSTNAME_OVERRIDE'],
    peersConfigFilePath: process.env['PEERS_CONFIG_FILE_PATH'],
    queryPeerIndex: parseInt(process.env['QUERY_PEER_INDEX'] || '0', 10),
    channelName: process.env['CHANNEL_NAME'],
    chaincodeId: process.env['CHAINCODE_ID']
  })
}

module.exports = getConfig
