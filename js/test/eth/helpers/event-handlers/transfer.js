'use strict'

const BN = require('bn.js')
const chalk = require('chalk')
const decodeData = require('./decode-data')
const u = require('../../../../util')

async function handleTransfer(
  platformState,
  secretStore,
  emitter,
  txHash,
  in0sn,
  in1sn,
  out0cm,
  out1cm,
  out0data,
  out1data,
  nextRoot
) {
  const outputs = [ [ out0cm, out0data ], [ out1cm, out1data ] ]
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
    u.buf2hex(txHash),
    [ in0sn, in1sn ],
    [ out0cm, out1cm ],
    nextRoot
  )
  console.log(
    chalk.grey(
      [
        'TRANSFER',
        `SN 0 ${u.buf2hex(in0sn)}`,
        `SN 1 ${u.buf2hex(in1sn)}`,
        `CM 0 ${u.buf2hex(out0cm)}`,
        `CM 1 ${u.buf2hex(out1cm)}`,
        `New Merkle tree root ${u.buf2hex(nextRoot)}`
      ].join('\n')
    )
  )

  emitter.emit('transferProcessed', txHash)
}

module.exports = handleTransfer
