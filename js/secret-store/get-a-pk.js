const u = require('../util')

function getAPk(state) {
  return u.hex2buf(state.get('a_pk'))
}

module.exports = getAPk
