const assert = require('assert')
const BN = require('bn.js')

const buf2hex = buf => '0x' + buf.toString('hex')

function checkBN(x) {
  assert(BN.isBN(x))
}

function checkBuf(buf, len) {
  assert(Buffer.isBuffer(buf))
  if (len !== undefined) {
    assert(buf.length === len)
  }
}

function checkString(input) {
  assert(
    Object.prototype.toString.call(input) === '[object String]',
    `Input is not a string: ${input}`
  )
}
const hex2buf = hex => Buffer.from(hex.substr(2), 'hex')

function shorthex(buf) {
  checkBuf(buf)
  if (buf.length <= 4) {
    return buf2hex(buf)
  } else {
    const str = `0x${buf.slice(0, 2).toString('hex')}...${buf.slice(-2).toString('hex')}`
    return str
  }
}

function parseG1Point(data) {
  const X = data[0]
  const Y = data[1]
  return [ X, Y ]
}

function parseG2Point(data) {
  const X = [ data[1], data[0] ]
  const Y = [ data[3], data[2] ]
  return [ X, Y ]
}

const wait = ms => new Promise(resolve => setTimeout(resolve, ms))

module.exports = {
  buf2hex,
  checkBN,
  checkBuf,
  checkString,
  hex2buf,
  parseG1Point,
  parseG2Point,
  wait,
  shorthex
}
