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
const conv = require('./conversion')

function makeSecretStore(privateKey, publicKey, noteInfos) {
  u.checkBuf(privateKey, 32)
  u.checkBuf(publicKey, 32)

  let cms
  if (noteInfos) {
    cms = Immutable.Map()
    noteInfos.forEach(e => {
      cms = cms.set(
        u.buf2hex(e.cm),
        conv.stringifyNote(e.a_pk, e.rho, e.r, e.v)
      )
    })
  } else {
    cms = Immutable.Map()
  }

  let state = Immutable.Map({
    privateKey: u.buf2hex(privateKey),
    publicKey: u.buf2hex(publicKey),
    cms
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
