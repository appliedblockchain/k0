function getAvailableNotes(cms) {
  return cms.entrySeq().filter(entry => {
    const [key, val] = entry
    return val.sn === undefined
  })
}

module.exports = getAvailableNotes
