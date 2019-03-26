const immutable = require('immutable')
const makeStateList = require('./state-list')
const makeMT = require('./mt')
const u = require('../util')

async function makeState(serverPort = 4000) {
  console.log('MT server port', serverPort)
  const mt = await makeMT(serverPort)
  let stateList
  reset()

  async function add(snapshotLabel, newCMList, newSNList, expectedNewRoot) {
    u.checkString(snapshotLabel)
    u.checkBuf(expectedNewRoot, 32)
    let newState = stateList.getLatest()
    console.log("new state", newState)
    for (let i = 0; i < newSNList.length; i++) {
      const currentSNList = newState.get('snList')
      newState = newState.set('snList', currentSNList.push(newCMList[i]))
    }
    for (let i = 0; i < newCMList.length; i++) {
      const currentCMList = newState.get('cmList')
      console.log(newState)
      console.log(currentCMList)
      newState = newState.set('cmList', currentCMList.push(newCMList[i]))
    }
    let newRoot
    // Add CMs to Merkle tree
    for (let i = 0; i < newCMList.length; i++) {
      const resp = await mt.add(newCMList[i])
      console.log('resp', resp)
      newRoot = resp.newRoot
    }
    // Check if new root of Merkle tree matches expected new root
    assert(newRoot.equals(expectedNewRoot))
    stateList.add(snapshotLabel, newState)
  }

  async function reset() {
    stateList = makeStateList()
    const state = immutable.Map({
      cmList: immutable.List(),
      snList: immutable.List()
    })
    stateList.add('initialState', state)
    await mt.reset()
  }

  const root = mt.root

  return {
    add,
    reset,
    root
  }
}

module.exports = makeState