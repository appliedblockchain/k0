'use strict'

const _ = require('lodash')
const assert = require('assert')
const asyncFs = require('../async-fs')
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
  const mtPath = path.join(contractsDir, 'MerkleTree.sol')
  await asyncFs.copyFile(
    path.join(
      path.parse(module.filename).dir, '..', '..', 'sol', 'MerkleTree.sol'
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


  const outputPath = path.join(contractsDir, 'output.json')
  const command = `solc --combined-json abi,bin MerkleTree.sol > output.json`
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
  //console.log(result)
  return {
    MerkleTree: extractContractArtefacts(result, 'MerkleTree'),
    AdditionVerifier: extractContractArtefacts(result, 'AdditionVerifier'),
    InclusionVerifier: extractContractArtefacts(result, 'InclusionVerifier')
  }
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

describe('Merkle tree', function () {

  this.timeout(100000)
  let web3, contract, mtEngine

  before(async () => {
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
    const merkleTreeAddress = await deploy(
      web3,
      contractArtefacts.MerkleTree.abi,
      contractArtefacts.MerkleTree.bytecode,
      50000000,
      [additionVerifierAddress, inclusionVerifierAddress, await util.pack256Bits(initialRoot)]
    )
    contract = new web3.eth.Contract(
      contractArtefacts.MerkleTree.abi,
      merkleTreeAddress
    )
  })

  it('happy path works', async function () {
    const secrets = []
    for (let i = 0; i < 4; i++) {
      const prefRootResult = await mtEngine.request('root', [])
      const r = randomBytesHex()
      const sn = randomBytesHex()
      secrets[i] = [r, sn]
      const hashResponse = await mtEngine.request('hash', [r, sn])
      const leaf = hashResponse.result
      console.log(hashResponse)
      const response = await mtEngine.request('simulateAddition', [leaf])
      console.log(response)
      const simulation = response.result;
      const leafElems = await util.pack256Bits(leaf)
      const data = contract.methods.add(
        leafElems,
        simulation.newRoot,
        ...simulation.proof
      ).encodeABI()
      const receipt = await sendTransaction(web3, contract._address, data)
      assert(receipt.status)
      assert(receipt.logs.length === 1)
      // event "Verified(string)"
      assert(receipt.logs[0].topics[0] === '0x3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e39')
      const additionResponse = await mtEngine.request('add', [leaf])
      console.log(`Added leaf ${i}: ${leaf}`)
      console.log(`New root: ${additionResponse.result.newRoot}`)
      for (let j = 0; j <= i; j++) {
        const [ r, sn ] = secrets[j]
        const inclusionProofResponse = await mtEngine.request('proveInclusion', [ j, r, sn ])
        console.log(inclusionProofResponse)
        const proof = inclusionProofResponse.result
        const snPacked = await util.pack256Bits(sn)
        console.log({ r, sn, snPacked, proof })
        console.log(contract.methods)
        const x = contract.methods.verifyKnowledgeOfLeafSecrets(snPacked, ...proof)
        const data = x.encodeABI()
        const receipt = await sendTransaction(web3, contract._address, data)
        assert(receipt.status)
        assert(receipt.logs.length === 1)
        // event "Verified(string)"
        assert(receipt.logs[0].topics[0] === '0x3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e39')
        console.log(`Proved inclusion of leaf ${j}.`)
      }
      console.log()
    }
  })

}) 
