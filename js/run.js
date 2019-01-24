const assert = require('assert')
const compile = require('./compile')
const crypto = require('crypto')
const deploy = require('./deploy')
const asyncFs = require('./async-fs')
const path = require('path')
const proofFromFile = require('./proof-from-file')
const vkFromFile = require('./vk-from-file')
const vkToSol = require('./vk-to-sol')
const Web3 = require('web3')

process.on('unhandledRejection', error => {
  console.error(error.stack)
  console.error('unhandledRejection', error.message);
});

async function run() {
  if (process.argv.length !== 5) {
    console.error('Need exactly 3 arguments (path to vk, path to proof, public inputs)')
    process.exit(1)
  }

  const tmpDir = process.env.TMP_DIR || '/tmp'

  // Initialise web3
  const web3 = new Web3(process.env.ETHEREUM_JSONRPC_ENDPOINT || 'http://localhost:8545/')
  const addresses = await web3.eth.getAccounts()

  // Read vk, proof and Solidity contract source code template
  const templateFilePath = path.join(path.parse(module.filename).dir, '..', 'sol', 'Verifier.sol')
  const [ vk, proof, contractTemplate ] = await Promise.all([
    vkFromFile(process.argv[2]),
    proofFromFile(process.argv[3]),
    asyncFs.readTextFile(templateFilePath)
  ])

  // Put vk into contract source, write to temporary file
  const vkSolSnippet = vkToSol(...vk)
  const contractSource = contractTemplate.replace('____VERIFYING_KEY_BODY____', vkSolSnippet)
  const tmpFilePath = path.join(tmpDir, `${crypto.randomBytes(32).toString('hex')}.sol`)
  await asyncFs.writeTextFile(tmpFilePath, contractSource, 'utf8')

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
  const contract = new web3.eth.Contract(contractArtefacts.abi, contractAddress)

  // Verify proof on chain
  const proofTxObj = contract.methods.verifyProof(...proof, JSON.parse(process.argv[4]))
  const result = await proofTxObj.send({
    from: addresses[0],
    gas: 50000000
  })

  assert(result.status && result.events.Verified)

  console.log(`Proof verified. Cost of verification: ${result.gasUsed} gas.`)
}

run().catch(console.error)
