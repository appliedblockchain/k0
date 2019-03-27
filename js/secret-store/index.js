const addNoteInfo = require('./add-note-info')
const getAvailableNotes = require('./get-available-notes')
const Immutable = require('immutable')
const u = require('../util')

function makeSecretStore(a_sk) {
  u.checkBuf(a_sk, 32)
  cms = Immutable.Map()
  return {
    addNoteInfo: (cm, rho, r, v) => cms = addNoteInfo(cms, cm, rho, r, v),
    getAvailableNotes: () => getAvailableNotes(cms),
    getSecretKey: () => a_sk,
    print: () => console.log(cms)
  }
}

module.exports = makeSecretStore