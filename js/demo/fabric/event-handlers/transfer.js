const BN = require('bn.js')
const chalk = require('chalk')
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

async function handleTransfer(platformState, secretStore, txnid, in0sn, in1sn, out0cm, out1cm, out0data, out1data, nextRoot) {

  const outputs = [
    [ u.hex2buf(out0cm), u.hex2buf(out0data) ],
    [ u.hex2buf(out1cm), u.hex2buf(out1data) ]
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
    [ u.hex2buf(in0sn), u.hex2buf(in1sn) ],
    [ u.hex2buf(out0cm), u.hex2buf(out1cm) ],
    u.hex2buf(nextRoot)
  )
  console.log(chalk.grey([
    `TRANSFER`,
    `SN 0 ${in0sn}`,
    `SN 1 ${in1sn}`,
    `CM 0 ${out0cm}`,
    `CM 1 ${out1cm}`,
    `New Merkle tree root ${nextRoot}`
  ].join('\n')))
}

module.exports = handleTransfer
