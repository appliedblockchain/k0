const jayson = require('jayson/promise')
const add = require('./add')
const path = require('./path')
const reset = require('./reset')
const root = require('./root')
const simulateAddition = require('./simulate-addition')

async function makeMT(endpoint = 'http://localhost:4100') {
  const jc = jayson.client.http(endpoint)
  return {
    add: add.bind(null, jc),
    path: path.bind(null, jc),
    reset: reset.bind(null, jc),
    root: root.bind(null, jc),
    simulateAdd: simulateAddition.bind(null, jc)
  }
}

module.exports = makeMT
