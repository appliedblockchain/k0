'use strict'

const asyncFs = require('./async-fs')
const compile = require('./compile')
const crypto = require('crypto')
const deploy = require('./deploy')
const generateVerifierContract = require('./generate-verifier-contract')
const path = require('path')
const tmpDir = process.env.TMP_DIR || '/tmp'

async function generateAndDeployContract(web3, pathToVk) {
  const tmpFilePath = path.join(tmpDir, `${crypto.randomBytes(32).toString('hex')}.sol`)
  await generateVerifierContract(pathToVk, tmpFilePath)
  // Compile the contract
  const contractArtefacts = await compile(tmpFilePath)

  // Delete the temporary file
  await asyncFs.unlink(tmpFilePath)
  // Deploy the contract
  const contractAddress = await deploy(
    web3,
    contractArtefacts.abi,
    contractArtefacts.bytecode,
    50000000
  )
  return new web3.eth.Contract(contractArtefacts.abi, contractAddress)

}

module.exports = generateAndDeployContract
