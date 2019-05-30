'use strict'

const BN = require('bn.js')
const u = require('../../../util')

function decodeData(data) {
  u.checkBuf(data, 176)
  return {
    a_pk: data.slice(0, 32),
    rho: data.slice(32, 64),
    r: data.slice(64, 112),
    v: new BN(data.slice(112, 176).toString('hex'), 'hex', 'le')
  }
}

async function handleTransfer(platformState, secretStore, txnid, in0sn, in1sn,
                              out0cm, out1cm, out0data, out1data, nextRoot) {

  const outputs = [
    [ out0cm, out0data ],
    [ out1cm, out1data ]
  ]

  for (let i = 0; i < 2; i = i + 1) {
    const info = decodeData(outputs[i][1])
    if (info.a_pk.equals(secretStore.getPublicKey())) {
      secretStore.addNoteInfo(
        outputs[i][0],
        info.a_pk,
        info.rho,
        info.r,
        info.v
      )
    }
  }

  await platformState.add(
    txnid,
    [ in0sn, in1sn ],
    [ out0cm, out1cm ],
    nextRoot
  )
}

module.exports = handleTransfer
