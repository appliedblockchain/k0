const handleDeposit = require('./deposit')
const handleTransfer = require('./transfer')
const EventEmitter = require('events')

function initEventHandlers(platformState, secretStore, platform, k0) {
  const emitter = new EventEmitter()
  platform.on(
    'deposit',
    handleDeposit.bind(null, platformState, secretStore, k0, emitter)
  )
  platform.on(
    'transfer',
    handleTransfer.bind(null, platformState, secretStore, k0, emitter)
  )

  return emitter
}

module.exports = initEventHandlers
