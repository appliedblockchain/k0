const crypto = require('crypto')
const u = require('../util')

async function generatePaymentData(server, secretStore, v) {
  u.checkBN(v)
  const rho = crypto.randomBytes(32)
  const r = crypto.randomBytes(48)
  const cm = await server.cm(secretStore.getPublicKey(), rho, r, v)
  return { rho, r, cm }
}

module.exports = generatePaymentData
