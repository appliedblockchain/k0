const assert = require('assert')
const compileContract = require('./helpers/compile-contract')
const crypto = require('crypto')
const deploy = require('../deploy')
const execAsync = require('../exec-async')
const path = require('path')
const proofFromFile = require('../proof-from-file')
const sendTransaction = require('../send-transaction')
const Web3 = require('web3')
const inquirer = require('inquirer')
const clear = require('clear')
const globalUtil = require('../util')
const BN = require('bn.js')

const { env } = process
const baseDir = path.join(__dirname, '..', '..')
const cppDir = process.env.CPP_DIR || path.join(baseDir, 'cpp')
const cppUtilDir = process.env.CPP_UTIL_DIR || path.join(cppDir, 'build', 'src')

async function convertVk(vkPath, vkAltPath) {
  const executablePath = path.join(cppUtilDir, 'convert_vk')
  const command = `${executablePath} ${vkPath} ${vkAltPath}`
  return execAsync(command)
}

async function convertProof(proofPath, proofAltPath) {
  const executablePath = path.join(cppUtilDir, 'convert_proof')
  const command = `${executablePath} ${proofPath} ${proofAltPath}`
  return execAsync(command)
}

function paths(label) {
  const tmpDir = process.env.TMP_DIR || path.join('/', 'tmp')

  return {
    tmpDir,
    baseDir,
    cppDir,
    cppUtilDir: path.join(cppDir, 'build', 'src'),
    pk: path.join(tmpDir, `${label}_pk`),
    vk: path.join(tmpDir, `${label}_vk`),
    vkAlt: path.join(tmpDir, `${label}_vk_alt`),
    proof: path.join(tmpDir, `${label}_proof`),
    proofAlt: path.join(tmpDir, `${label}_proof_alt`)
  }
}

async function pack256Bits(hex) {
  globalUtil.checkString(hex)
  let executablePath

  if (env.CIRCLECI) {
    executablePath = `docker run appliedblockchain/zktrading-pack:${env.CIRCLE_BRANCH}-${env.CIRCLE_SHA1}`
  } else {
    executablePath = path.join(cppUtilDir, 'pack_256_bits')
  }

  const command = `${executablePath} ${hex}`
  const result = await execAsync(command)
  return result.stdout.trim().split(',')
}

async function verifyOnChainWithProof(web3, contract, proof, args, expectSuccess) {
  const data = contract.methods.verifyProof(...proof, args).encodeABI()
  const receipt = await sendTransaction(web3, contract._address, data)
  if (expectSuccess) {
    assert(receipt.status)
    assert(receipt.logs.length === 1)
    // event "Verified(string)"
    assert(receipt.logs[0].topics[0] === '0x3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e39')
    console.log(`Proof verified. Cost of verification: ${receipt.gasUsed} gas.`)
  } else {
    assert(receipt.status)
    assert(receipt.logs.length === 0)
  }
}

async function verifyOnChain(web3, contract, proofAltPath, args, expectSuccess) {
  const proof = await proofFromFile(proofAltPath)
  await verifyOnChainWithProof(web3, contract, proof, args, expectSuccess)
}

function randomBytes(len) {
  return '0x' + crypto.randomBytes(len).toString('hex')
}

async function sha256Instance() {
  const pyScriptPath = path.join(baseDir, 'python', 'sha256_instance.py')
  const pyResult = await execAsync(`python ${pyScriptPath}`)
  return pyResult.stdout.split(',')
}

function initWeb3() {
  const endpointFromEnv = process.env.ETHEREUM_JSONRPC_ENDPOINT
  return new Web3(endpointFromEnv || 'ws://localhost:8546/')
}

async function deployContract(web3, artefacts, params = [], account) {
  const contractAddress = await deploy(
    web3,
    artefacts.abi,
    artefacts.bytecode,
    50000000,
    params,
    account
  )
  return new web3.eth.Contract(artefacts.abi, contractAddress)
}

async function deployStandardContract(web3, contractName, account = null, params = []) {
  const artefacts = await compileContract(contractName)
  const contractAddress = await deploy(
    web3,
    artefacts.abi,
    artefacts.bytecode,
    50000000,
    params,
    account
  )
  return new web3.eth.Contract(artefacts.abi, contractAddress)
}

function toUnits(input) {
  assert(BN.isBN(input))
  const ten = new BN('10', 10)
  const decimals = new BN('8', 10)
  const multiplier = ten.pow(decimals)
  return input.mul(multiplier)
}

function fromUnits(input) {
  assert(BN.isBN(input))
  const ten = new BN('10', 10)
  const decimals = new BN('8', 10)
  const divisor = ten.pow(decimals)
  return input.div(divisor)
}

function randomBytesHex(len = 32) {
  return '0x' + crypto.randomBytes(len).toString('hex')
}

async function prompt() {
  if (process.env.PROMPT === 'true') {
    console.log()
    const response = await inquirer.prompt({
      type: 'confirm',
      name: 'continue',
      message: 'Continue?'
    })
    if (!response.continue) {
      process.exit(0)
    }
    console.log()
  }
}

module.exports = {
  clear,
  convertVk,
  convertProof,
  deployContract,
  deployStandardContract,
  initWeb3,
  randomBytes,
  paths,
  pack256Bits,
  prompt,
  proofFromFile,
  randomBytesHex,
  sha256Instance,
  verifyOnChain,
  verifyOnChainWithProof,
  toUnits,
  fromUnits
}
