const jayson = require('jayson/promise')
const add = require('./add')
const prepare_deposit = require('./prepare_deposit')
const prf_addr = require('./prf_addr')
const ready = require('./ready')
const reset = require('./reset')
const root = require('./root')

function client(serverPort = 4000) {
  const jc = jayson.client.http({ port: serverPort })
  return {
    add: add.bind(null, jc),
    prepare_deposit: prepare_deposit.bind(null, jc),
    prf_addr: prf_addr.bind(null, jc),
    ready: ready.bind(null, jc),
    reset: reset.bind(null, jc),
    root: root.bind(null, jc)
  }
}

module.exports = client