'use strict'
const execAsync = require('./exec-async')
const asyncFs = require('./async-fs')
const crypto = require('crypto')

async function compile(path) {
  const tmpFilePath = `/tmp/${crypto.randomBytes(32).toString('hex')}.json`
  const command = `solc --combined-json abi,bin ${path} > ${tmpFilePath}`
  const options = { maxBuffer: 1024 * 1024 }
  const { stdout, stderr } = await execAsync(command, options)
  if (stderr) {
    console.log(stderr)
  }
  const json = await asyncFs.readTextFile(tmpFilePath)
  await asyncFs.unlink(tmpFilePath)
  const result = JSON.parse(json)
  const contractInfo = result.contracts[`${path}:Verifier`]
  return {
    abi: JSON.parse(contractInfo.abi),
    bytecode: '0x' + contractInfo.bin
  }
}

module.exports = compile
