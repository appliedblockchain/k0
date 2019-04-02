const handleMint = require('./event-handlers/mint')
const handleTransfer = require('./event-handlers/transfer')

function initEventHandlers(platformState, secretStore, platform) {
  console.log({platform})
  platform.on('mint', handleMint.bind(null, platformState))
}

module.exports = initEventHandlers
