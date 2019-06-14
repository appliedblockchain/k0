
async function prepareWithdrawal(
  server,
  platformState,
  secretStore,
  cm,
  address
) {

  const noteMktIndex = await platformState.indexOfCM(cm)

  console.inspect({ noteMktIndex })

  const noteInfos = await secretStore.getNoteInfo(cm)

  const a_sk = await secretStore.getASk()

  const params = [
    noteMktIndex,
    a_sk,
    noteInfos.rho,
    noteInfos.r,
    noteInfos.v,
    address
  ]

  console.inspect(params)

  const result = await server.prepare_withdrawal(...params)

  return result
}

module.exports = prepareWithdrawal
