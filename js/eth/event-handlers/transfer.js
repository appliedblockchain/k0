const ethUtil = require('../util')
const u = require('../../util')

async function handleTransfer(eventEmitter, event) {
  const [ sn_in_0, sn_in_1, cm_out_0, cm_out_1, new_root ] = await Promise.all(
    [ 'sn_in_0', 'sn_in_1', 'cm_out_0', 'cm_out_1', 'new_root' ].map(label => {
      return ethUtil.unpack(event.returnValues[label])
    })
  )
  const [ data_out_0, data_out_1 ] = await Promise.all(
    [ 'data_out_0', 'data_out_1' ].map(label => {
      return u.hex2buf(event.returnValues[label])
    })
  )
  console.log(event.returnValues.callee)
  eventEmitter.emit(
    'transfer',
    u.hex2buf(event.transactionHash),
    sn_in_0,
    sn_in_1,
    cm_out_0,
    cm_out_1,
    data_out_0,
    data_out_1,
    new_root
  )
}

module.exports = handleTransfer
