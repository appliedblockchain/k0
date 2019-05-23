const handleDeposit = require('./deposit')
const handleTransfer = require('./transfer')

function initEventHandlers(platformState, secretStore, platform) {
  platform.on('deposit', handleDeposit.bind(platform, platformState))
  platform.on(
    'transfer',
    handleTransfer.bind(platform, platformState, secretStore)
  )
}

module.exports = initEventHandlers
