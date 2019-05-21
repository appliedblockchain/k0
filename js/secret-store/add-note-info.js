const u = require('../util')
const conv = require('./conversion')

function addNoteInfo(state, cm, a_pk, rho, r, v) {
  u.checkBuf(cm, 32)
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  const cmString = u.buf2hex(cm)

  const cmsBefore = state.get('cms')
  const cmsAfter = cmsBefore.set(
    cmString,
    conv.stringifyNote(a_pk, rho, r, v)
  )

  return state.set('cms', cmsAfter)
}

module.exports = addNoteInfo
