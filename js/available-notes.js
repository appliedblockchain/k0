'use strict'

async function availableNotes(chainState, secretStore) {
  const myNotes = await secretStore.getNoteInfos()
  // TODO check which SNs have actually already been used
  // const usedSNs = await chainState.getSNs()
}
