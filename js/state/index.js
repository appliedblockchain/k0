const immutable = require('immutable')
const stateList = require('./state-list')

async function makeState() {
  const stateList = makeStateList()
  let state = immutable.Map({
    cmList: immutable.List(),
    snList: immutable.List()
  })
  statesList.add(initialState)
  async function add(snapshotLabel, newCMList, newSNList) {
    let newState = state
    for(let i = 0; i < newSNList.length; i++) {
      const currentSNList = newState.snList
      newState = newState.set('snList', currentSNList.add(newCMList[i]))
    }
    for(let i = 0; i < newCMList.length; i++) {
      const currentCMList = newState.cmList
      newState = newState.set('cmList', currentCMList.add(newCMList[i]))
    }
    // TODO update merkle tree
    state = newState
    statesList.add(state)
  }
  return { add }
}

return makeState