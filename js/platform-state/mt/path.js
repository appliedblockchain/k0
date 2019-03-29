const request = require('./request')
const u = require('../../util')

async function path(jc, idx) {
  u.checkBN(idx)
  const resp = await request(jc, 'path', [ idx.toString() ])
  return resp.map(u.hex2buf)
}

module.exports = path
