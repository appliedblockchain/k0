function makeLinkedList() {
  let latest = null
  function add(content) {
    latest = { content, prev: latest }
  }
  function getLatest() {
    if (latest === null) {
      return null
    } else {
      return latest.content
    }
  }
  function iterator() {
    let current = latest
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

  return { add, getLatest, iterator }
}

module.exports = makeLinkedList