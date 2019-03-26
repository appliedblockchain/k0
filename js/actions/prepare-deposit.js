const BN = require('bn.js')
const crypto = require('crypto')
const u = require('../util')

async function prepareDeposit(server, keys, v) {
  u.checkBuf(keys.a_pk, 32)
  u.checkBN(v)
  const rho = crypto.randomBytes(32)
  const r = crypto.randomBytes(48)
  const data = await server.prepare_deposit(keys.a_pk, rho, r, v)
  console.log(data)
  return {
    rho,
    r,
    cm: u.hex2buf(data.cm),
    k: u.hex2buf(data.k),
    nextRoot: u.hex2buf(data.nextRoot),
    // TODO hex2buf
    additionProof: data.additionProof,
    commitmentProof: data.commitmentProof
  }
}

module.exports = prepareDeposit
