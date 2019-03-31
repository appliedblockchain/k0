const u = require('../util')
const conv = require('./conversion')

function addSNToNote(state, cm, sn) {
  u.checkBuf(cm, 32)
  u.checkBuf(sn, 32)
  return state.setIn(['cms', u.buf2hex(cm), 'sn' ], u.buf2hex(sn))
}

module.exports = addSNToNote
