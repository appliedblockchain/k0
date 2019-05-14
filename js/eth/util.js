'use strict'

const execAsync = require('../exec-async')
const path = require('path')
const BN = require('bn.js')
const globalUtil = require('../util')

const baseDir = path.join(__dirname, '..', '..')
const cppDir = process.env.CPP_DIR || path.join(baseDir, 'cpp')
const cppUtilDir = process.env.CPP_UTIL_DIR || path.join(cppDir, 'build', 'src')

async function pack256Bits(buf) {
  globalUtil.checkBuf(buf, 32)
  const hex = globalUtil.buf2hex(buf)

  let executablePath
  if (process.env.CIRCLECI) {
    executablePath = 'docker run appliedblockchain/zktrading-pack'
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
  globalUtil.checkBN(val1)
  globalUtil.checkBN(val2)

  let executablePath
  if (process.env.CIRCLECI) {
    executablePath = 'docker run appliedblockchain/zktrading-unpack'
  } else {
    executablePath = path.join(cppUtilDir, 'unpack_256_bits')
  }

  const command = `${executablePath} ${val1.toString()} ${val2.toString()}`
  const result = await execAsync(command)
  return globalUtil.hex2buf(result.stdout.trim())
}

module.exports = {
  pack256Bits,
  unpack,
  unpack256Bits
}
