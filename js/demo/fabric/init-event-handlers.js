const handleMint = require('./event-handlers/mint')
const handleTransfer = require('./event-handlers/transfer')

function initEventHandlers(platformState, secretStore, platform) {
  platform.on('mint', handleMint.bind(null, platformState))
  platform.on('transfer', handleTransfer.bind(null, platformState, secretStore))
}

module.exports = initEventHandlers
