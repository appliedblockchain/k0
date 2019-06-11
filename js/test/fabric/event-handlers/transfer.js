'use strict'

const BN = require('bn.js')
const decodeData = require('./decode-data')
const u = require('../../../util')

async function handleTransfer(platformState, secretStore, txnid, in0sn, in1sn,
                              out0cm, out1cm, out0data, out1data, nextRoot) {
  const outputs = [
    [ out0cm, out0data ],
    [ out1cm, out1data ]
  ]

  for (let i = 0; i < 2; i = i + 1) {
    const info = decodeData(outputs[i][1])
    if (info.a_pk.equals(secretStore.getAPk())) {
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
