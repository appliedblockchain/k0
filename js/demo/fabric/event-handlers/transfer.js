const BN = require('bn.js')
const chalk = require('chalk')
const u = require('../../util')

function decodeData(data) {
  u.checkBuf(data, 176)
  return {
    a_pk: data.slice(0, 32),
    rho: data.slice(32, 64),
    r: data.slice(64, 112),
    v: new BN(data.slice(112, 176).toString('hex'), 'hex', 'le')
  }
}

async function handleTransfer(platformState, secretStore, txHash, in0sn, in1sn,
                              out0cm, out1cm, out0data, out1data, nextRoot,
                              callee) {
  const outputs = [
    [ out0cm, out0data ],
    [ out1cm, out1data ]
  ]
  for(let i = 0; i < 2; i = i + 1) {
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
  console.log(chalk.grey([
    `TRANSFER`,
    `SN 0 ${u.buf2hex(in0sn)}`,
    `SN 1 ${u.buf2hex(in1sn)}`,
    `CM 0 ${u.buf2hex(out0cm)}`,
    `CM 1 ${u.buf2hex(out1cm)}`,
    `New Merkle tree root ${u.buf2hex(nextRoot)}`
  ].join('\n')))
}

module.exports = handleTransfer
