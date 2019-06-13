const u = require('@appliedblockchain/k0-util')

function getPkEnc(state) {
  return u.hex2buf(state.get('pk_enc'))
}

module.exports = getPkEnc
