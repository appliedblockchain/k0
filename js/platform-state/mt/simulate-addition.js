const BN = require('bn.js')
const request = require('./request')
const u = require('../../util')

async function add(jc, cm) {
  u.checkBuf(cm, 32)
  const resp = await request(jc, 'simulateAddition', [ u.buf2hex(cm) ])
  return {
    address: new BN(resp.address),
    nextRoot: u.hex2buf(resp.nextRoot),
    path: resp.path.map(u.hex2buf)
  }
}

module.exports = add
