const u = require('@appliedblockchain/k0-util')

function getASk(state) {
  return u.hex2buf(state.get('a_sk'))
}

module.exports = getASk
