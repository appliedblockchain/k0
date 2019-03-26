const BN = require('bn.js')
const request = require('./request')
const u = require('../../util')

async function add(jc, cm) {
  u.checkBuf(cm, 32)
  const resp = await request(jc, 'add', [ u.buf2hex(cm) ])
  return {
    address: new BN(resp.address),
    newRoot: u.hex2buf(resp.newRoot)
  }
}

module.exports = add