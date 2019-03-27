const request = require('./request')
const u = require('../../util')

async function root(jc) {
  const res = await request(jc, 'root')
  return u.hex2buf(res)
}

module.exports = root