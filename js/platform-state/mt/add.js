const BN = require('bn.js')
const request = require('./request')
const u = require('../../util')

async function add(jc, cm) {
  u.checkBuf(cm, 32)
  const resp = await request(jc, 'add', [ u.buf2hex(cm) ])
  console.log(resp)
  return {
    address: new BN(resp.address),
    nextRoot: u.hex2buf(resp.nextRoot)
  }
}

module.exports = add