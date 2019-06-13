const u = require('@appliedblockchain/k0-util')

// Returns the address (concatenation of a_pk and pk_enc) for the stored secret
// key a_sk
function getAddress(state) {
  const a_pk = u.hex2buf(state.get('a_pk'))
  const pk_enc = u.hex2buf(state.get('pk_enc'))
  return Buffer.concat([ a_pk, pk_enc ])
}

module.exports = getAddress
