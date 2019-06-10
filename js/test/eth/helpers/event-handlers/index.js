const handleDeposit = require('./deposit')
const handleTransfer = require('./transfer')
const EventEmitter = require('events')

function initEventHandlers(platformState, secretStore, platform) {
  const emitter = new EventEmitter()
  platform.on(
    'deposit',
    handleDeposit.bind(null, platformState, secretStore, emitter)
  )
  platform.on(
    'transfer',
    handleTransfer.bind(null, platformState, secretStore, emitter)
  )

  return emitter
}

module.exports = initEventHandlers
