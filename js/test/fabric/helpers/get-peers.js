'use strict'
const _ = require('lodash')
const fs = require('fs')
const log4js = require('log4js')
const logger = log4js.getLogger()
const path = require('path')
logger.level = process.env['LOG_LEVEL'] || 'info'
const readFile = require('./read-file')

function tlsCACertPath(domain) {
  return path.join(
    __dirname,
    '..',
    'network',
    'crypto-config',
    'peerOrganizations',
    domain,
    'tlsca',
    `tlsca.${domain}-cert.pem`
  )
}

function getPeers() {
  return [
    { url: 'grpcs://localhost:11751',
      eventUrl: 'grpcs://localhost:11753',
      tlsHostnameOverride: 'peer0.alpha.com',
      tlsCaCertPEM: readFile(tlsCACertPath('alpha.com'))
    },
    { url: 'grpcs://localhost:12751',
      eventUrl: 'grpcs://localhost:12753',
      tlsHostnameOverride: 'peer0.beta.com',
      tlsCaCertPEM: readFile(tlsCACertPath('beta.com'))
    },
    { url: 'grpcs://localhost:13751',
      eventUrl: 'grpcs://localhost:13753',
      tlsHostnameOverride: 'peer0.gamma.com',
      tlsCaCertPEM: readFile(tlsCACertPath('gamma.com'))
    }
  ]
}

module.exports = getPeers
