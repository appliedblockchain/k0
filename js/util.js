const assert = require('assert')
const BN = require('bn.js')
const hexOfBuffer = require('@appliedblockchain/helpers/hex-of-buffer')
const hex0xOfHex = require('@appliedblockchain/helpers/hex0x-of-hex')
const bufferOfHex0x = require('@appliedblockchain/helpers/buffer-of-hex0x')

const buf2hex = buf => hex0xOfHex(hexOfBuffer(buf))

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
		Object.prototype.toString.call(input) === "[object String]",
		`Input is not a string: ${input}`
	)
}
const hex2buf = bufferOfHex0x

function shorthex(buf) {
  checkBuf(buf)
  if (buf.length <= 4) {
    return buf2hex(buf)
  } else {
    const str = `0x${buf.slice(0,4).toString('hex')}...${buf.slice(-4).toString('hex')}`
    return str
  }
}

function parseG1Point(data) {
	const X = data[0]
	const Y = data[1]
	return [X, Y]
}

function parseG2Point(data) {
	const X = [data[1], data[0]]
	const Y = [data[3], data[2]]
	return [X, Y]
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
