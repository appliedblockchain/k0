const asyncFs = require('./async-fs')
const path = require('path')
const vkFromFile = require('./vk-from-file')
const vkToSol = require('./vk-to-sol')

// Path to openzeppelin contracts
const contractsSrcDir = path.join(
  __dirname,
  'node_modules',
  '@appliedblockchain',
  'k0-eth-contracts'
)

// Generates a verifier contract from a verifying key
async function generateVerifierContract(pathToVk, filePath, contractName) {
  // Read vk and Solidity contract source code template
  const templateFilePath = path.join(contractsSrcDir, 'GenericVerifier.sol')
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

module.exports = generateVerifierContract
