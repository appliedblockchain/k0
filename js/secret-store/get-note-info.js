const BN = require('bn.js')
const u = require('../util')

function getNoteInfo(state, cm) {
  u.checkBuf(cm, 32)
  const noteInfo = state.get('cms').get(u.buf2hex(cm))

  if (noteInfo !== undefined) {
    const data = {
      a_pk: u.hex2buf(noteInfo.get('a_pk')),
      rho: u.hex2buf(noteInfo.get('rho')),
      r: u.hex2buf(noteInfo.get('r')),
      v: new BN(noteInfo.get('v'))
    }
    if (noteInfo.get('sn')) {
      data.sn = u.hex2buf(noteInfo.get('sn'))
    }
    return data
  } else {
    return null
  }
}

module.exports = getNoteInfo
