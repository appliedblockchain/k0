const u = require('@appliedblockchain/k0-util')

function getSkEnc(state) {
  return u.hex2buf(state.get('sk_enc'))
}

module.exports = getSkEnc
