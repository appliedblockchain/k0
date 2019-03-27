const u = require('../util')
const conv = require('./conversion')

function addNoteInfo(cms, cm, rho, r, v) {
  u.checkBuf(cm, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  const cmString = u.buf2hex(cm)
  return cms.set(
    cmString,
    conv.stringifyNote(rho, r, v)
  )
}

module.exports = addNoteInfo