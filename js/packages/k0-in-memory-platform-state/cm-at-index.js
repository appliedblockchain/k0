const u = require('@appliedblockchain/k0-util')

function cmAtIndex(stateList, idx) {
  u.checkBN(idx)
  const state = stateList.getLatest()
  const cmHex = state.get('cmList').get(idx.toNumber())
  return u.hex2buf(cmHex)
}

module.exports = cmAtIndex
