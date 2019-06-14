const u = require('../util')

function getSkEnc(state) {
  return u.hex2buf(state.get('sk_enc'))
}

module.exports = getSkEnc
