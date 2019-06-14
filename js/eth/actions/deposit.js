'use strict'

const BN = require('bn.js')
const flattenProof = require('../flatten-proof')
const signTransaction = require('../sign-transaction')
const u = require('../../util')
const assert = require('assert')

async function deposit(
  web3,
  mvppt,
  privateKey,
  v,
  k,
  cm,
  data,
  nextRoot,
  commitmentProof,
  additionProof
) {
  u.checkBuf(privateKey, 32)
  u.checkBN(v)
  u.checkBuf(k, 32)
  u.checkBuf(cm, 32)
  u.checkBuf(data, 136)
  u.checkBuf(nextRoot, 32)

  const commitmentProofCompact = flattenProof(commitmentProof)
  const additionProofCompact = flattenProof(additionProof)

  const params = [
    v.toString(),
    (await u.pack256Bits(k)).map(u.bn2string),
    (await u.pack256Bits(cm)).map(u.bn2string),
    u.buf2hex(data),
    (await u.pack256Bits(nextRoot)).map(u.bn2string),
    commitmentProofCompact.map(u.bn2string),
    additionProofCompact.map(u.bn2string)
  ]

  return signTransaction(
    web3,
    u.hex2buf(mvppt._address),
    u.hex2buf(mvppt.methods.deposit(...params).encodeABI()),
    5000000,
    privateKey
  )
}

module.exports = deposit
