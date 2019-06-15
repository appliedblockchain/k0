const u = require('@appliedblockchain/k0-util')
const BN = require('bn.js')

function indexOfCM(stateList, cm) {
  u.checkBuf(cm, 32)
  const cmHex = u.buf2hex(cm)
  const state = stateList.getLatest()
  const idx = state.get('cmList').indexOf(cmHex)
  if (idx === -1) {
    return null
  } else {
    return new BN(idx)
  }
}

module.exports = indexOfCM
