'use strict'

const u = require('@appliedblockchain/k0-util')

function addSNToNote(state, cm, sn) {
  u.checkBuf(cm, 32)
  u.checkBuf(sn, 32)
  const newState = state.setIn([ 'cms', u.buf2hex(cm), 'sn' ], u.buf2hex(sn))
  return newState
}

module.exports = addSNToNote
