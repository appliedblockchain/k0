'use strict'

const u = require('../util')

async function prepareTransfer(
  server,
  platformState,
  secretStore,
  in0idx,
  in1idx,
  out0data,
  out1data,
  calleeAddress
) {
  u.checkBN(in0idx)
  u.checkBN(in1idx)

  const mtRoot = await platformState.merkleTreeRoot()
  const privateKey = secretStore.getPrivateKey()

  const inputs = [ in0idx, in1idx ].map(idx => {
    u.checkBN(idx)
    const cm = platformState.cmAtIndex(idx)
    const info = secretStore.getNoteInfo(cm)

    return {
      address: idx,
      a_sk: privateKey,
      ...info
    }
  })

  const outputs = [ out0data, out1data ].map(data => {
    u.checkBuf(data.a_pk, 32)
    u.checkBuf(data.rho, 32)
    u.checkBuf(data.r, 48)
    u.checkBN(data.v)
    return data
  })

  if (calleeAddress) {
    u.checkBuf(calleeAddress, 20)
  }

  const params = [
    mtRoot,
    inputs[0].address,
    inputs[0].a_sk,
    inputs[0].rho,
    inputs[0].r,
    inputs[0].v,
    await platformState.cmPath(in0idx),
    inputs[1].address,
    inputs[1].a_sk,
    inputs[1].rho,
    inputs[1].r,
    inputs[1].v,
    await platformState.cmPath(in1idx),
    outputs[0].a_pk,
    outputs[0].rho,
    outputs[0].r,
    outputs[0].v,
    outputs[1].a_pk,
    outputs[1].rho,
    outputs[1].r,
    outputs[1].v,
    calleeAddress
  ]
  return server.prepareTransfer(...params)
}

module.exports = prepareTransfer
