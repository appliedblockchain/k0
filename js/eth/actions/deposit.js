const BN = require('bn.js')
const flattenProof = require('../flatten-proof')
const signTransaction = require('../sign-transaction')
const u = require('../../util')
const ethU = require('../util')


async function deposit(web3, mvppt, privateKey, v, k, cm, nextRoot, commitmentProof,
                       additionProof) {
  assert(Buffer.isBuffer(privateKey) && privateKey.length == 32)
  assert(BN.isBN(v))
  assert(Buffer.isBuffer(k) && k.length === 32)
  assert(Buffer.isBuffer(cm) && k.length === 32)
  assert(Buffer.isBuffer(nextRoot))
  console.log(commitmentProof)
  const commitmentProofCompact = flattenProof(commitmentProof)
  const additionProofCompact = flattenProof(additionProof)

  const params = [
    v.toString(),
    (await ethU.pack256Bits(k)).map(u.stringifyBN),
    (await ethU.pack256Bits(cm)).map(u.stringifyBN),
    (await ethU.pack256Bits(nextRoot)).map(u.stringifyBN),
    commitmentProofCompact.map(u.stringifyBN),
    additionProofCompact.map(u.stringifyBN)
  ]

  console.log(params)

  return signTransaction(
    web3,
    u.hex2buf(mvppt._address),
    u.hex2buf(mvppt.methods.deposit(...params).encodeABI()),
    5000000,
    privateKey
  )
}

module.exports = deposit
