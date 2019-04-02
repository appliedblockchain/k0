const immutable = require('immutable')
const cmAtIndex = require('./cm-at-index')
const makeStateList = require('./state-list')
const makeMT = require('./mt')
const u = require('../util')
const currentState = require('./current-state')

async function makePlatformState(serverPort = 4100) {
  const mt = await makeMT(serverPort)
  let stateList
  reset()

  async function add(snapshotLabel, newSNList, newCMList, expectedNextRoot) {
    console.log('ADDING')
    console.log('args', { snapshotLabel, newSNList, newCMList, expectedNextRoot})
    u.checkString(snapshotLabel)
    newCMList.forEach(cm => u.checkBuf(cm, 32))
    newSNList.forEach(sn => u.checkBuf(sn, 32))
    if (expectedNextRoot !== undefined) {
      u.checkBuf(expectedNextRoot, 32)
    }
    let newState = stateList.getLatest()
    console.log('state', newState)
    for (let i = 0; i < newSNList.length; i++) {
      const currentSNList = newState.get('snList')
      newState = newState.set('snList', currentSNList.push(u.buf2hex(newSNList[i])))
    }
    for (let i = 0; i < newCMList.length; i++) {
      const currentCMList = newState.get('cmList')
      newState = newState.set('cmList', currentCMList.push(u.buf2hex(newCMList[i])))
    }
    let nextRoot

    // Add CMs to Merkle tree
    for (let i = 0; i < newCMList.length; i++) {
      const resp = await mt.add(newCMList[i])
      nextRoot = resp.nextRoot
    }

    if (expectedNextRoot !== undefined) {
      // Check if new root of Merkle tree matches expected new root
      assert(
        nextRoot.equals(expectedNextRoot),
        `New root is ${u.buf2hex(nextRoot)}, expected ${u.buf2hex(expectedNextRoot)}`
      )
    }
    console.log('state after', newState)
    stateList.add(snapshotLabel, newState)
  }

  async function reset() {
    stateList = makeStateList()
    const state = immutable.Map({
      cmList: immutable.List(),
      snList: immutable.List(),
      cmInfo: immutable.Map()
    })
    stateList.add('initialState', state)
    await mt.reset()
  }

  async function rollbackTo(label) {
    stateList.rollbackTo(label)
    await mt.reset()
    const state = stateList.getLatest()
    const cmList = state.get('cmList').toJS()
    for (let i = 0; i < cmList.length; i++) {
      await mt.add(u.hex2buf(cmList[i]))
    }
  }

  const merkleTreeRoot = mt.root

  const simulateMerkleTreeAddition = mt.simulateAdd

  return {
    add,
    cmAtIndex: idx => cmAtIndex(stateList, idx),
    cmPath: mt.path,
    reset,
    merkleTreeRoot,
    simulateMerkleTreeAddition,
    currentState: () => currentState(stateList),
    currentStateLabel: stateList.getLatestLabel,
    rollbackTo
  }
}

module.exports = makePlatformState
