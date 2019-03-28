'use strict'

const addNoteInfo = require('./add-note-info')
const getAvailableNotes = require('./get-available-notes')
const getPrivateKey = require('./get-private-key')
const getPublicKey = require('./get-public-key')
const getNoteInfo = require('./get-note-info')
const Immutable = require('immutable')
const u = require('../util')
const spit = require('./spit')
const slurp = require('./slurp')

function makeSecretStore(importDump) {
  let state = slurp(importDump)
  return {
    addNoteInfo: (cm, a_pk, rho, r, v) => state = addNoteInfo(state, cm, a_pk, rho, r, v),
    getAvailableNotes: () => getAvailableNotes(state),
    getPrivateKey: () => getPrivateKey(state),
    getPublicKey: () => getPublicKey(state),
    getNoteInfo: cm => getNoteInfo(state, cm),
    spit: () => spit(state)
  }
}

module.exports = makeSecretStore
