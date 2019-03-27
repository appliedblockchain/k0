const immutable = require('immutable')
const makeStateList = require('./state-list')
const makeMT = require('./mt')
const u = require('../util')

async function makePlatformState(serverPort = 4100) {
  console.log('MT server port', serverPort)
  const mt = await makeMT(serverPort)
  let stateList
  reset()

  async function add(snapshotLabel, newCMList, newSNList, expectednextRoot) {
    u.checkString(snapshotLabel)
    u.checkBuf(expectednextRoot, 32)
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
    let nextRoot
    // Add CMs to Merkle tree
    for (let i = 0; i < newCMList.length; i++) {
      const resp = await mt.add(newCMList[i])
      console.log('resp', resp)
      nextRoot = resp.nextRoot
    }
    // Check if new root of Merkle tree matches expected new root
    assert(nextRoot.equals(expectednextRoot))
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

  function print() {
    console.log(stateList.getLatest())
  }

  const merkleTreeRoot = mt.root

  const simulateMerkleTreeAddition = mt.simulateAdd

  return {
    add,
    print,
    reset,
    merkleTreeRoot,
    simulateMerkleTreeAddition
  }
}

module.exports = makePlatformState