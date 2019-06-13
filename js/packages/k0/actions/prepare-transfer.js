'use strict'

const encryptNote = require('./helpers/encrypt-note')
const u = require('@appliedblockchain/k0-util')

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
  const a_sk = secretStore.getASk()

  const inputs = [ in0idx, in1idx ].map(idx => {
    u.checkBN(idx)
    const cm = platformState.cmAtIndex(idx)
    const info = secretStore.getNoteInfo(cm)

    return {
      address: idx,
      a_sk,
      ...info
    }
  })

  const outputs = [ out0data, out1data ].map(data => {
    u.checkBuf(data.address, 64)
    u.checkBuf(data.rho, 32)
    u.checkBuf(data.r, 48)
    u.checkBN(data.v)
    return data
  })

  if (calleeAddress) {
    u.checkBuf(calleeAddress, 20)
  }

  const ciphertexts = await Promise.all(outputs.map(o => {
    return encryptNote(server, o.address, o.v, o.rho, o.r)
  }))
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
    outputs[0].address.slice(0, 32),
    outputs[0].rho,
    outputs[0].r,
    outputs[0].v,
    outputs[1].address.slice(0, 32),
    outputs[1].rho,
    outputs[1].r,
    outputs[1].v,
    calleeAddress
  ]
  const result = await server.prepareTransfer(...params)
  result.output_0_ciphertext = ciphertexts[0]
  result.output_1_ciphertext = ciphertexts[1]
  return result
}

module.exports = prepareTransfer
