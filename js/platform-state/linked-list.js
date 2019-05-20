function makeLinkedList() {
  const listState = {
    latest: null
  }

  function add(content) {
    listState.latest = { content, prev: listState.latest }
  }

  function getLatest() {
    let current = listState.latest
    if (current === null) {
      return null
    } else {
      return current.content
    }
  }

  function rollbackTo(content) {
    let current = listState.latest
    while (current.content !== content) {
      if (current.prev === null) {
        return Error('set listState.latest failed')
      }
      current = current.prev
    }
    listState.latest = current
  }

  function iterator() {
    let current = listState.latest
    function next() {
      if (current === null) {
        return { done: true }
      } else {
        const elementToReturn = current.content
        current = current.prev
        return { value: elementToReturn, done: false }
      }
    }
    return { next }
  }

  return { add, getLatest, rollbackTo, iterator }
}

module.exports = makeLinkedList
