'use strict'

const Immutable = require('immutable')
const addNoteInfo = require('./add-note-info')
const addSNToNote = require('./add-sn-to-note')
const getAddress = require('./get-address')
const getAPk = require('./get-a-pk')
const getASk = require('./get-a-sk')
const getAvailableNotes = require('./get-available-notes')
const getNoteInfo = require('./get-note-info')
const getSkEnc = require('./get-sk-enc')
const getPkEnc = require('./get-pk-enc')
const makeClient = require('../client')
const spit = require('./spit')
const u = require('../util')
const conv = require('./conversion')

async function makeSecretStore(serverPort, a_sk, noteInfos) {
  assert(!isNaN(serverPort), `server port ${serverPort} is not a number`)
  u.checkBuf(a_sk, 32)
  const server = makeClient(serverPort)
  const { a_pk, sk_enc, pk_enc }= await server.deriveKeys(a_sk)

  let cms = Immutable.Map() // eslint-disable-line
  if (noteInfos) {
    noteInfos.forEach(e => {
      cms = cms.set(
        u.buf2hex(e.cm),
        conv.stringifyNote(e.a_pk, e.rho, e.r, e.v)
      )
    })
  }

  // eslint-disable-next-line
  let state = Immutable.Map({
    a_sk: u.buf2hex(a_sk),
    a_pk: u.buf2hex(a_pk),
    sk_enc: u.buf2hex(sk_enc),
    pk_enc: u.buf2hex(pk_enc),
    cms
  })

  return {
    addNoteInfo: (cm, a_pk, rho, r, v) => {
      state = addNoteInfo(state, cm, a_pk, rho, r, v)
    },
    addSNToNote: (cm, sn) => {
      state = addSNToNote(state, cm, sn)
    },
    getAddress: () => getAddress(state),
    getAvailableNotes: () => getAvailableNotes(state),
    getASk: () => getASk(state),
    getAPk: () => getAPk(state),
    getSkEnc: () => getSkEnc(state),
    getPkEnc: () => getPkEnc(state),
    getNoteInfo: cm => getNoteInfo(state, cm),
    spit: () => spit(state)
  }
}

module.exports = makeSecretStore
