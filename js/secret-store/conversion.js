const immutable = require('immutable')
const u = require('../util')

function stringifyNote(rho, r, v) {
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48),
  u.checkBN(v)
  return immutable.Map({
    rho: u.buf2hex(rho),
    r: u.buf2hex(r),
    v: v.toString()
  })
}

module.exports = { stringifyNote }