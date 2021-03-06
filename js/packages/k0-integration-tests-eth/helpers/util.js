const assert = require('assert')
const compileContract = require('./compile-contract')
const crypto = require('crypto')
const deploy = require('./deploy')
const path = require('path')
const sendTransaction = require('./send-transaction')
const Web3 = require('web3')
const inquirer = require('inquirer')
const clear = require('clear')
const globalUtil = require('@appliedblockchain/k0-util')
const BN = require('bn.js')

const { env } = process
const baseDir = path.join(__dirname, '..', '..', '..', '..')
const cppDir = process.env.CPP_DIR || path.join(baseDir, 'cpp')
const cppUtilDir = process.env.CPP_UTIL_DIR || path.join(cppDir, 'build', 'src')

function paths(label) {
  const tmpDir = process.env.TMP_DIR || path.join('/', 'tmp', 'k0keys')
  const baseDir = path.join(__dirname, '..', '..') // eslint-disable-line
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

function randomBytes(len) {
  return '0x' + crypto.randomBytes(len).toString('hex')
}

function initWeb3() {
  const endpointFromEnv = process.env.ETHEREUM_JSONRPC_ENDPOINT
  return new Web3(endpointFromEnv || 'ws://localhost:8546/')
}

async function deployContract(web3, artefacts, params = [], account) {
  return deploy(
    web3,
    artefacts.abi,
    artefacts.bytecode,
    50000000,
    params,
    account
  )
}

async function deployStandardContract(web3,
  contractName,
  account = null,
  params = []
) {
  const artefacts = await compileContract(contractName)
  return deploy(
    web3,
    artefacts.abi,
    artefacts.bytecode,
    50000000,
    params,
    account
  )
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

async function sleep(seconds = 0, message = '') {
  if (message) {
    console.log('Sleeping for ${sec} seconds')
  }
  return new Promise(a => {
    setTimeout(sec * 1000, a)
  })
}

const ZERO_ADDRESS = '0x0000000000000000000000000000000000000000'

function awaitEvent(emitter, eventName, timeoutSecs = 20) {
  return new Promise((accept, reject) => {
    const timeout = setTimeout(() => {
      // TODO unregister the event listener?
      reject(new Error(`Event was not emitted within ${timeoutSecs} seconds`))
    }, timeoutSecs * 1000)
    emitter.once(eventName, (event) =>{
      accept(event)
      clearTimeout(timeout)
    })
  })
}

module.exports = {
  awaitEvent,
  sleep,
  clear,
  deployContract,
  deployStandardContract,
  initWeb3,
  randomBytes,
  paths,
  prompt,
  randomBytesHex,
  toUnits,
  fromUnits,
  ZERO_ADDRESS
}
