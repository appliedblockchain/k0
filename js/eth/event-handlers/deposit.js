const ethUtil = require('../util')
const u = require('../../util')

async function handleDeposit(eventEmitter, event) {
  const cmStrings = event.returnValues.cm
  const [ cm, newRoot ] = await Promise.all([
    ethUtil.unpack(event.returnValues.cm),
    ethUtil.unpack(event.returnValues.new_root)
  ])
  await eventEmitter.emit(
    'deposit',
    u.hex2buf(event.transactionHash),
    cm,
    newRoot
  )
}

module.exports = handleDeposit
