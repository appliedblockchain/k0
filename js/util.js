'use strict'

const BN = require('bn.js')
const assert = require('assert')
const execAsync = require('./exec-async')
const path = require('path')

const baseDir = path.join(__dirname, '..')
const cppDir = process.env.CPP_DIR || path.join(baseDir, 'cpp')
const cppUtilDir = process.env.CPP_UTIL_DIR || path.join(cppDir, 'build', 'src')
const { env } = process

const buf2hex = buf => '0x' + buf.toString('hex')

function checkBN(x) {
	assert(BN.isBN(x))
}

function checkBuf(buf, len) {
	assert(Buffer.isBuffer(buf), `Not a buffer: ${buf}`)
  if (len !== undefined) {
    assert(buf.length === len, `Buffer has length ${buf.length}, expected ${len}`)
  }
}

function checkFp2(coord) {
	assert(Object.keys(coord).length === 2)
	checkBN(coord.c0)
	checkBN(coord.c1)
}

function checkG1Affine(point) {
	assert(Object.keys(point).length === 2)
	checkBN(point.x)
	checkBN(point.y)
}

function checkG2Affine(point) {
	assert(Object.keys(point).length === 2)
	checkFp2(point.x)
	checkFp2(point.y)
}

function checkG1Jacobian(point) {
	assert(Object.keys(point).length === 3)
	checkBN(point.x)
	checkBN(point.y)
	checkBN(point.z)
}

function checkG2Jacobian(point) {
	assert(Object.keys(point).length === 3)
	checkFp2(point.x)
	checkFp2(point.y)
	checkFp2(point.z)
}

function checkProofAffine(proof) {
	checkG1Affine(proof.g_A.g)
	checkG1Affine(proof.g_A.h)
	checkG2Affine(proof.g_B.g)
	checkG1Affine(proof.g_B.h)
	checkG1Affine(proof.g_C.g)
	checkG1Affine(proof.g_C.h)
	checkG1Affine(proof.g_H)
	checkG1Affine(proof.g_K)
}

function checkProofJacobian(proof) {
	checkG1Jacobian(proof.g_A.g)
	checkG1Jacobian(proof.g_A.h)
	checkG2Jacobian(proof.g_B.g)
	checkG1Jacobian(proof.g_B.h)
	checkG1Jacobian(proof.g_C.g)
	checkG1Jacobian(proof.g_C.h)
	checkG1Jacobian(proof.g_H)
	checkG1Jacobian(proof.g_K)
}

function checkString(input) {
	assert(
		Object.prototype.toString.call(input) === "[object String]",
		`Input is not a string: ${input}`
	)
}

const hex2buf = hex => Buffer.from(hex.substr(2), 'hex')

function shorthex(buf) {
  checkBuf(buf)
  if (buf.length <= 4) {
    return buf2hex(buf)
  } else {
    const str = `0x${buf.slice(0,2).toString('hex')}...${buf.slice(-2).toString('hex')}`
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

function bn2string(bn) {
	checkBN(bn)
	return bn.toString()
}

function string2bn(str) {
	checkString(str)
	return new BN(str)
}

const wait = ms => new Promise(resolve => setTimeout(resolve, ms))

async function pack256Bits(buf) {
  checkBuf(buf, 32)
  const hex = buf2hex(buf)

  let executablePath
  if (env.CIRCLECI) {
    executablePath = `docker run appliedblockchain/zktrading-pack:${env.CIRCLE_BRANCH}-${env.CIRCLE_SHA1}`
  } else {
    executablePath = path.join(cppUtilDir, 'pack_256_bits')
  }

  const command = `${executablePath} ${hex}`
  const result = await execAsync(command)
  return result.stdout.trim().split(',').map(str => new BN(str))
}

function unpack(decStringPair) {
  return unpack256Bits(
    new BN(decStringPair[0]),
    new BN(decStringPair[1])
  )
}

async function unpack256Bits(val1, val2) {
  checkBN(val1)
  checkBN(val2)

  let executablePath
  if (process.env.CIRCLECI) {
    executablePath = `docker run appliedblockchain/zktrading-unpack:${env.CIRCLE_BRANCH}-${env.CIRCLE_SHA1}`
  } else {
    executablePath = path.join(cppUtilDir, 'unpack_256_bits')
  }

  const command = `${executablePath} ${val1.toString()} ${val2.toString()}`
  const result = await execAsync(command)
  return hex2buf(result.stdout.trim())
}

module.exports = {
	bn2string,
	buf2hex,
	checkBN,
	checkBuf,
	checkProofAffine,
	checkProofJacobian,
	checkString,
	hex2buf,
	shorthex,
	string2bn,
	wait,
  pack256Bits,
  parseG1Point,
  parseG2Point,
  unpack,
  unpack256Bits
}
