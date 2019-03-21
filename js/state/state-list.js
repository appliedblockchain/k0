const makeLinkedList = require('./linked-list')

function makeStateList() {
  const linkedList = makeLinkedList()
  function add(label, state) {
    linkedList.add({
      label, state
    })
  }
  return { add }
}

module.exports = makeStateList