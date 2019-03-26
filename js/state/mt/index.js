const jayson = require('jayson/promise')
const add = require('./add')
const reset = require('./reset')
const root = require('./root')

async function makeMT(serverPort = 4000) {
  const jc = jayson.client.http({ port: serverPort })
  return {
    add: add.bind(null, jc),
    reset: reset.bind(null, jc),
    root: root.bind(null, jc)
  }
}

module.exports = makeMT