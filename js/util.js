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
	assert(len !== undefined)
	assert(Buffer.isBuffer(buf) && buf.length === len)
}

const hex2buf = bufferOfHex0x

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
	hex2buf,
  parseG1Point,
  parseG2Point,
	wait
}
