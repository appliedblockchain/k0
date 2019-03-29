const flattenProof = require('../flatten-proof')
const signTransaction = require('../sign-transaction')
const u = require('../../util')
const ethU = require('../util')

async function transfer(web3, mvppt, privateKey, in_0_sn, in_1_sn, out_0_cm, out_1_cm, nextRoot, calleeAddress, proof) {
  [ privateKey, in_0_sn, in_1_sn, out_0_cm, out_1_cm, nextRoot ].map(buf => {
    u.checkBuf(buf, 32)
  })
  u.checkBuf(calleeAddress, 20)
  const proofCompact = flattenProof(proof)

  const params = [
    (await ethU.pack256Bits(in_0_sn)).map(bn => bn.toString()),
    (await ethU.pack256Bits(in_1_sn)).map(bn => bn.toString()),
    (await ethU.pack256Bits(out_0_cm)).map(bn => bn.toString()),
    (await ethU.pack256Bits(out_1_cm)).map(bn => bn.toString()),
    (await ethU.pack256Bits(nextRoot)).map(bn => bn.toString()),
    u.buf2hex(calleeAddress),
    proofCompact
  ]

  return signTransaction(
    web3,
    u.hex2buf(mvppt._address),
    u.hex2buf(mvppt.methods.transfer(...params).encodeABI()),
    5000000,
    privateKey
  )
}

module.exports = transfer
