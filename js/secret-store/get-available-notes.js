const BN = require('bn.js')
const u = require('../util')

function getAvailableNotes(state) {
  return state
    .cms
    .entrySeq()
    .filter(entry => {
      const [ val ] = entry
      return val.sn === undefined
    })
    .toArray()
    .map(entry => {
      const [ key, val ] = entry
      return {
        cm: u.hex2buf(key),
        a_pk: u.hex2buf(val.get('a_pk')),
        rho: u.hex2buf(val.get('rho')),
        r: u.hex2buf(val.get('r')),
        v: new BN(val.get('v'))
      }
    })
}

module.exports = getAvailableNotes
