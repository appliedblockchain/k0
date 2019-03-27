const assert = require('assert')
const makeLinkedList = require('./linked-list')
const Immutable = require('immutable')
const u = require('../util')

function makeStateList() {
  const linkedList = makeLinkedList()

  function add(label, state) {
    u.checkString(label)
    assert(Immutable.Map.isMap(state))
    linkedList.add({
      label, state
    })
  }
  function getLatest() {
    const entry = linkedList.getLatest()
    return entry.state
  }
  return { add, getLatest }
}

module.exports = makeStateList