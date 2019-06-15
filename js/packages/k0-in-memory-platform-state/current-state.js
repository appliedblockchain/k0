const u = require('@appliedblockchain/k0-util')

function currentState(stateList) {
  const state = stateList.getLatest()
  return {
    cmList: state.get('cmList').toJS().map(u.hex2buf),
    snList: state.get('snList').toJS().map(u.hex2buf)
  }
}

module.exports = currentState
