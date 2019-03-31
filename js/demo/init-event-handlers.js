const handleDeposit = require('./event-handlers/deposit')
const handleTransfer = require('./event-handlers/transfer')

function initEventHandlers(platformState, secretStore, platform) {
  platform.on('deposit', handleDeposit.bind(null, platformState))
  platform.on('transfer', handleTransfer.bind(null, platformState, secretStore))
}

module.exports = initEventHandlers
