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
}

module.exports = prepareDeposit