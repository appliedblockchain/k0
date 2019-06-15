const flattenProof = require('../flatten-proof')
const signTransaction = require('../sign-transaction')
const u = require('@appliedblockchain/k0-util')

async function transfer(
  web3,
  mvppt,
  privateKey,
  in_0_sn,
  in_1_sn,
  out_0_cm,
  out_1_cm,
  out_0_data,
  out_1_data,
  nextRoot,
  calleeAddress,
  proof
) {
  [ privateKey, in_0_sn, in_1_sn, out_0_cm, out_1_cm, nextRoot ].map(buf => {
    u.checkBuf(buf, 32)
  })

  u.checkBuf(out_0_data)
  u.checkBuf(out_1_data)
  u.checkBuf(calleeAddress, 20)
  const proofCompact = flattenProof(proof)

  const params = [
    (await u.pack256Bits(in_0_sn)).map(bn => bn.toString()),
    (await u.pack256Bits(in_1_sn)).map(bn => bn.toString()),
    (await u.pack256Bits(out_0_cm)).map(bn => bn.toString()),
    (await u.pack256Bits(out_1_cm)).map(bn => bn.toString()),
    u.buf2hex(out_0_data),
    u.buf2hex(out_1_data),
    (await u.pack256Bits(nextRoot)).map(bn => bn.toString()),
    web3.utils.toChecksumAddress(u.buf2hex(calleeAddress)),
    proofCompact.map(bn => bn.toString())
  ]

  return signTransaction(
    web3,
    u.hex2buf(mvppt.address),
    u.hex2buf(mvppt.methods.transfer(...params).encodeABI()),
    5000000,
    privateKey
  )
}

module.exports = transfer
