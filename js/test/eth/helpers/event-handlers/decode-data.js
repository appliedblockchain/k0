'use strict'

const BN = require('bn.js')
const u = require('../../../../util')

function decodeData(data) {
  u.checkBuf(data, 176)
  return {
    a_pk: data.slice(0, 32),
    rho: data.slice(32, 64),
    r: data.slice(64, 112),
    v: new BN(data.slice(112, 176).toString('hex'), 'hex', 'le')
  }
}

module.exports = decodeData
