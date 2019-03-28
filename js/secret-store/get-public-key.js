const u = require('../util')

function getPublicKey(state) {
  return u.hex2buf(state.get('publicKey'))
}

module.exports = getPublicKey
