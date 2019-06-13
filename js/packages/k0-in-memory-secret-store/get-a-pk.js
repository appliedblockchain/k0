const u = require('@appliedblockchain/k0-util')

function getAPk(state) {
  return u.hex2buf(state.get('a_pk'))
}

module.exports = getAPk
