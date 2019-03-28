const u = require('../util')

function getPrivateKey(state) {
  return u.hex2buf(state.get('privateKey'))
}

module.exports = getPrivateKey
