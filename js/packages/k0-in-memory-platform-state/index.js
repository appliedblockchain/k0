'use strict'

const cmAtIndex = require('./cm-at-index')
const currentState = require('./current-state')
const immutable = require('immutable')
const indexOfCM = require('./index-of-cm')
const makeMT = require('./mt')
const makeStateList = require('./state-list')
const u = require('@appliedblockchain/k0-util')
const assert = require('assert')

async function makePlatformState(mtServerEndpoint = 'http://localhost:4100') {
  const mt = await makeMT(mtServerEndpoint)
  let stateList
  await reset()

  async function add(snapshotLabel, newSNList, newCMList, expectedNextRoot) {
    u.checkString(snapshotLabel)
    newCMList.forEach(cm => u.checkBuf(cm, 32))
    newSNList.forEach(sn => u.checkBuf(sn, 32))
    if (expectedNextRoot !== undefined) {
      u.checkBuf(expectedNextRoot, 32)
    }
    let newState = stateList.getLatest()
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

  async function print(label) {
    console.log(label, '************************')
    console.log(label, 'mt root' , await mt.root())
    stateList.print(label)
  }

  return {
    add,
    cmAtIndex: idx => cmAtIndex(stateList, idx),
    indexOfCM: cm => indexOfCM(stateList, cm),
    cmPath: mt.path,
    reset,
    merkleTreeRoot,
    simulateMerkleTreeAddition,
    currentState: () => currentState(stateList),
    currentStateLabel: () => stateList.getLatestLabel(),
    rollbackTo,
    print
  }
}

module.exports = makePlatformState
