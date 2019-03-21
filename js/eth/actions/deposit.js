const flattenProof = require('../flatten-proof')
const sendTransaction = require('../send-transaction')
const u = require('../../util')

async function deposit(web3, mvppt, account, v, k, cm, commitmentProof,
  additionProof) {
  assert(BN.isBN(v))
  assert(Buffer.isBuffer(k) && k.length === 32)
  assert(Buffer.isBuffer(cm) && k.length === 32)
  assert(Buffer.isBuffer(nextRoot))
  const commitmentProofCompact = flattenProof(commitmentProof)
  const additionProofCompact = flattenProof(additionProof)

  const params = [
    v.toString(),
    await u.pack256Bits(u.buf2hex(k)),
    await u.pack256Bits(u.buf2hex(cm)),
    await u.pack256Bits(u.buf2hex(nextRoot)),
    commitmentProofCompact,
    additionProofCompact
  ]

  return sendTransaction(
    web3,
    mvppt._address,
    mvppt.methods.deposit(...params).encodeABI(),
    5000000,
    account
  )
}

module.exports = deposit