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

  function getLatestLabel() {
    const entry = linkedList.getLatest()
    return entry.label
  }

  function rollbackTo(label) {
    const it = linkedList.iterator()
    let done = false
    while (!done) {
      const next = it.next()
      if (next.value.label === label) {
        linkedList.rollbackTo(next.value)
        return
      } else {
        done = next.done
      }
    }
    throw new Error(`Rollback failed. No state with label ${label} found.`)
  }

  return { add, getLatest, getLatestLabel, rollbackTo }
}

module.exports = makeStateList
