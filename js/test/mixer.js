'use strict'

const _ = require('lodash')
const assert = require('assert')
const asyncFs = require('../async-fs')
const compileContract = require('./helpers/compile-contract')
const crypto = require('crypto')
const deploy = require('../deploy')
const execAsync = require('../exec-async')
const jayson = require('jayson/promise')
const path = require('path')
const util = require('./util')
const expect = require('code').expect
const sendTransaction = require('../send-transaction')
const vkFromFile = require('../vk-from-file')
const vkToSol = require('../vk-to-sol')

if (process.env.MOCHA_MERKLE_TREE_HEIGHT === undefined) {
  console.log('Env var MOCHA_MERKLE_TREE_HEIGHT must be set.')
  process.exit(1)
}

const TREE_HEIGHT = parseInt(process.env.MOCHA_MERKLE_TREE_HEIGHT || '2', 10)
const NUM_PEOPLE = 2

const tmpDir = '/tmp'

const paths = {
  additionPk: path.join(tmpDir, 'mt_addition_pk'),
  additionVkAlt: path.join(tmpDir, 'mt_addition_vk_alt'),
  inclusionPk: path.join(tmpDir, 'mt_inclusion_pk'),
  inclusionVkAlt: path.join(tmpDir, 'mt_inclusion_vk_alt')
}
function extractContractArtefacts(result, contractName) {
  const contractInfo = result.contracts[`${contractName}.sol:${contractName}`]
  return {
    abi: JSON.parse(contractInfo.abi),
    bytecode: '0x' + contractInfo.bin
  }
}

async function generateVerifierContractAlt(pathToVk, filePath, contractName) {
  // Read vk and Solidity contract source code template
  const templateFilePath = path.join(path.parse(module.filename).dir, '..', '..', 'sol', 'GenericVerifier.sol')
  const [vk, contractTemplate] = await Promise.all([
    vkFromFile(pathToVk),
    asyncFs.readTextFile(templateFilePath)
  ])

  // Put contract name and vk into contract source, write to temporary file
  const vkSolSnippet = vkToSol(...vk)
  let contractSource = contractTemplate.replace('____CONTRACT_NAME____', contractName)
  contractSource = contractSource.replace('____VERIFYING_KEY_BODY____', vkSolSnippet)

  await asyncFs.writeTextFile(filePath, contractSource, 'utf8')

}

async function compileContracts() {
  const contractsDir = path.join(tmpDir, crypto.randomBytes(32).toString('hex'))
  await asyncFs.mkdir(contractsDir)
  await generateVerifierContractAlt(
    paths.additionVkAlt,
    path.join(contractsDir, 'AdditionVerifier.sol'),
    'AdditionVerifier'
  )
  await generateVerifierContractAlt(
    paths.inclusionVkAlt,
    path.join(contractsDir, 'InclusionVerifier.sol'),
    'InclusionVerifier'
  )
  const mtPath = path.join(contractsDir, 'Mixer.sol')
  await asyncFs.copyFile(
    path.join(
      path.parse(module.filename).dir, '..', '..', 'sol', 'Mixer.sol'
    ),
    mtPath
  )
  const pairingPath = path.join(contractsDir, 'Pairing.sol')
  await asyncFs.copyFile(
    path.join(
      path.parse(module.filename).dir, '..', '..', 'sol', 'Pairing.sol'
    ),
    pairingPath
  )

  const ozDir = path.join(
    __dirname,
    '..',
    'node_modules',
    'openzeppelin-solidity'
  )
  const outputPath = path.join(contractsDir, 'output.json')
  const command = [
    `solc --combined-json abi,bin openzeppelin-solidity=${ozDir}`,
    `Mixer.sol > output.json`
  ].join(' ')
  const options = {
    cwd: contractsDir,
    maxBuffer: 1024 * 1024
  }
  const { stdout, stderr } = await execAsync(command, options)
  if (stderr) {
    console.log(stderr)
  }
  const json = await asyncFs.readTextFile(outputPath)
  // TODO Delete directory
  //await asyncFs.unlink(tmpFilePath)
  const result = JSON.parse(json)
  console.log(result)
  return {
    Mixer: extractContractArtefacts(result, 'Mixer'),
    AdditionVerifier: extractContractArtefacts(result, 'AdditionVerifier'),
    InclusionVerifier: extractContractArtefacts(result, 'InclusionVerifier')
  }
}

async function deployStandardContract(web3, contractName, account = null) {
  const artefacts = await compileContract(contractName)
  const contractAddress = await deploy(
    web3,
    artefacts.abi,
    artefacts.bytecode,
    50000000,
    [],
    account
  )
  return new web3.eth.Contract(artefacts.abi, contractAddress)
}

const wait = ms => new Promise(resolve => setTimeout(resolve, ms))

async function mtEngineReady(mtEngine) {
  let ready = false;

  process.stdout.write('Waiting for the server to become ready...')
  while (!ready) {
    const statusResponse = await mtEngine.request('status', [])
    ready = statusResponse.result.ready
    if (!ready) {
      process.stdout.write('.')
      await wait(1000)
    }
  }
  process.stdout.write('\n')
  console.log('Server ready.')
}

function randomBytesHex(len = 32) {
  return '0x' + crypto.randomBytes(len).toString('hex')
}

describe('Commitment-based mixer', function () {

  this.timeout(4 * 3600 * 1000)
  let web3, erc20, mixer, tokenMaster, depositors, mtEngine

  async function printBalances(addresses) {
    addresses = [ mixer._address, ...addresses ]
    console.log('Balances:')
    for (let i = 0; i < addresses.length; i++) {
      const balance = await erc20.methods.balanceOf(addresses[i]).call()
      console.log(addresses[i], web3.utils.fromWei(balance))
    }
  }

  before(async () => {

    web3 = util.initWeb3()
    tokenMaster = web3.eth.accounts.create()
    depositors = _.times(NUM_PEOPLE, () => web3.eth.accounts.create())

    erc20 = await deployStandardContract(web3, 'DollarCoin', tokenMaster)
    const moneyShower = await deployStandardContract(web3, 'MoneyShower')

    // Make some money
    const data = erc20.methods
      .mint(tokenMaster.address, web3.utils.toWei('1000000'))
      .encodeABI()

    await sendTransaction(web3, erc20._address, data, 5000000, tokenMaster)

    // Money to the people
    await sendTransaction(
      web3,
      erc20._address,
      erc20.methods.approve(
        moneyShower._address, web3.utils.toWei((NUM_PEOPLE).toString())
      ).encodeABI(),
      5000000,
      tokenMaster
    )
    await sendTransaction(
      web3,
      moneyShower._address,
      moneyShower.methods.transfer(
        erc20._address,
        _.map(depositors, 'address'),
        _.times(depositors.length, () => web3.utils.toWei('1'))
      ).encodeABI(),
      5000000,
      tokenMaster
    )
    const proverPort = parseInt(process.env.PROVER_PORT || '4000', 10)
    mtEngine = jayson.client.http({ port: proverPort })
    await mtEngineReady(mtEngine)
    await mtEngine.request('reset', [])
    const initialRootResponse = await mtEngine.request('root', [])
    const initialRoot = initialRootResponse.result
    web3 = util.initWeb3()
    const contractArtefacts = await compileContracts()
    const additionVerifierAddress = await deploy(
      web3,
      contractArtefacts.AdditionVerifier.abi,
      contractArtefacts.AdditionVerifier.bytecode,
      50000000
    )
    const inclusionVerifierAddress = await deploy(
      web3,
      contractArtefacts.InclusionVerifier.abi,
      contractArtefacts.InclusionVerifier.bytecode,
      50000000
    )
    const mixerAddress = await deploy(
      web3,
      contractArtefacts.Mixer.abi,
      contractArtefacts.Mixer.bytecode,
      50000000,
      [
        erc20._address,
        additionVerifierAddress,
        inclusionVerifierAddress,
        await util.pack256Bits(initialRoot)
      ]
    )
    mixer = new web3.eth.Contract(
      contractArtefacts.Mixer.abi,
      mixerAddress
    )

  })

  it('happy path works', async function () {
    const secrets = [], additionProvingTimes = [], inclusionProvingTimes = []

    printBalances(_.map(depositors, 'address'))

    for (let i = 0; i < NUM_PEOPLE; i++) {
      const account = depositors[i]

      await sendTransaction(
        web3,
        erc20._address,
        erc20.methods.approve(
          mixer._address, web3.utils.toWei('1')
        ).encodeABI(),
        5000000,
        account
      )

      const r = randomBytesHex()
      const sn = randomBytesHex()
      secrets[i] = [r, sn]
      const hashResponse = await mtEngine.request('hash', [r, sn])
      const leaf = hashResponse.result

      const timestampStart = Date.now()
      const response = await mtEngine.request('simulateAddition', [leaf])
      const proofDuration = Date.now() - timestampStart

      const simulation = response.result;
      const leafElems = await util.pack256Bits(leaf)
      const data = mixer.methods.payIn(
        leafElems,
        simulation.newRoot,
        ...simulation.proof
      ).encodeABI()
      const receipt = await sendTransaction(web3, mixer._address, data, 5000000, account)
      assert(receipt.status)

      const additionResponse = await mtEngine.request('add', [leaf])
      console.log(`Added leaf ${i}: ${leaf}`)
      console.log(`New root: ${additionResponse.result.newRoot}`)
      additionProvingTimes.push(proofDuration)

      const timesSum = additionProvingTimes.reduce((acc, val) => acc + val)
      const avg = timesSum / additionProvingTimes.length

      console.log([
        'Duration of addition proving operation:',
        `${Math.round(proofDuration/1000)}s`,
        `(avg: ${Math.round(avg / 1000)}s)`
      ].join(' '))

      await printBalances(_.map(depositors, 'address'))
      console.log()
    }

    const withdrawers = _.times(NUM_PEOPLE, () => web3.eth.accounts.create())

    for (let i = 0; i < NUM_PEOPLE; i++) {

      const account = withdrawers[i];

      const [r, sn] = secrets[i]
      const timestampStart = Date.now()
      const inclusionProofResponse = await mtEngine.request('proveInclusion', [i, r, sn])
      const proofDuration = Date.now() - timestampStart
      const proof = inclusionProofResponse.result
      const snPacked = await util.pack256Bits(sn)
      const x = mixer.methods.withdraw(snPacked, ...proof)
      const data = x.encodeABI()
      const receipt = await sendTransaction(web3, mixer._address, data, 5000000, account)
      assert(receipt.status)
      await printBalances(_.map([...depositors, ...withdrawers], 'address'))
      inclusionProvingTimes.push(proofDuration)

      const timesSum = inclusionProvingTimes.reduce((acc, val) => acc + val)
      const avg = timesSum / inclusionProvingTimes.length

      console.log([
        'Duration of inclusion proving operation:',
        `${Math.round(proofDuration/1000)}s`,
        `(avg: ${Math.round(avg / 1000)}s)`
      ].join(' '))
      console.log()
    }


  })


})