'use strict'

const BN = require('bn.js')
const u = require('@appliedblockchain/k0-util')

function decodeData(data) {
  u.checkBuf(data, 88)
  return {
    v: new BN(data.slice(0, 8).toString('hex'), 'hex', 'le'),
    rho: data.slice(8, 40),
    r: data.slice(40,88)
  }
}

async function decryptNote(server, secretStore, ciphertext) {
  const decryptionResult = await server.decryptNote(
    ciphertext,
    secretStore.getSkEnc(),
    secretStore.getPkEnc()
  )
  if (decryptionResult.success === false) {
    return {
      success: false
    }
  } else {
    const result = decodeData(decryptionResult.value)
    result.success = true
    return result
  }
}

module.exports = decryptNote
