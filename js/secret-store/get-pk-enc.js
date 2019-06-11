const u = require('../util')

function getPkEnc(state) {
  return u.hex2buf(state.get('pk_enc'))
}

module.exports = getPkEnc
