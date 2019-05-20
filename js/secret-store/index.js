'use strict'

const Immutable = require('immutable')
const addNoteInfo = require('./add-note-info')
const addSNToNote = require('./add-sn-to-note')
const getAvailableNotes = require('./get-available-notes')
const getNoteInfo = require('./get-note-info')
const getPrivateKey = require('./get-private-key')
const getPublicKey = require('./get-public-key')
const spit = require('./spit')
const u = require('../util')

function makeSecretStore(privateKey, publicKey, noteInfos) {
  u.checkBuf(privateKey, 32)
  u.checkBuf(publicKey, 32)
  if (noteInfos.length > 0) {
    throw new Error('noteInfo import not yet supported')
  }
  let state = Immutable.Map({
    privateKey: u.buf2hex(privateKey),
    publicKey: u.buf2hex(publicKey),
    cms: Immutable.Map()
  })

  return {
    addNoteInfo: (cm, a_pk, rho, r, v) => {
      state = addNoteInfo(state, cm, a_pk, rho, r, v)
    },
    addSNToNote: (cm, sn) => {
      state = addSNToNote(state, cm, sn)
    },
    getAvailableNotes: () => getAvailableNotes(state),
    getPrivateKey: () => getPrivateKey(state),
    getPublicKey: () => getPublicKey(state),
    getNoteInfo: cm => getNoteInfo(state, cm),
    spit: () => spit(state)
  }
}

module.exports = makeSecretStore
