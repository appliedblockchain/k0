'use strict'

const _ = require('lodash')

function getDevModePeers() {
  return _.range(1,4).map(function(i) {
    return {
      url: `grpc://localhost:1${i}751`,
      eventUrl: `grpc://localhost:1${i}753`
    }
  })
}

module.exports = getDevModePeers
