'use strict'

const _ = require('lodash')
const crypto = require('crypto')
const execAsync = require('../../exec-async')
const path = require('path')
const asyncFs = require('../../async-fs')
const vkFromFile = require('../../vk-from-file')
const vkToSol = require('../../vk-to-sol')

// Path to this project's contracts directory
const contractsSrcDir = path.join(
  path.parse(module.filename).dir,
  '..',
  '..',
  '..',
  'sol'
)

// Path to openzeppelin contracts
const ozDir = path.join(
  __dirname,
  '..',
  '..',
  'node_modules',
  'openzeppelin-solidity'
)

// Extracts abi and bytecode from solc output
function extractContractArtefacts(result, contractName) {
  const contractInfo = result.contracts[`${contractName}.sol:${contractName}`]
  return {
    abi: JSON.parse(contractInfo.abi),
    bytecode: '0x' + contractInfo.bin
  }
}

// Generates a verifier contract from a verifying key
async function generateVerifierContractAlt(pathToVk, filePath, contractName) {
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

async function compileContracts(tmpDir = '/tmp/k0keys') {
  const paths = {
    commitmentPk: path.join(tmpDir, 'commitment_pk'),
    commitmentVkAlt: path.join(tmpDir, 'commitment_vk_alt'),
    additionPk: path.join(tmpDir, 'addition_pk'),
    additionVkAlt: path.join(tmpDir, 'addition_vk_alt'),
    transferPk: path.join(tmpDir, 'transfer_pk'),
    transferVkAlt: path.join(tmpDir, 'transfer_vk_alt'),
    withdrawalPk: path.join(tmpDir, 'withdrawal_pk'),
    withdrawalVkAlt: path.join(tmpDir, 'withdrawal_vk_alt')
  }
  const contractsDir = path.join(tmpDir, crypto.randomBytes(32).toString('hex'))
  await asyncFs.mkdir(contractsDir)

  const verifiers = [
    'AdditionVerifier',
    'CommitmentVerifier',
    'WithdrawalVerifier',
    'TransferVerifier'
  ]

  const normalContracts = [
    'Callee',
    'CarToken',
    'CarTrade',
    'DollarCoin',
    'HiddenPriceCarTrade',
    'MoneyShower',
    'MVPPT',
    'Pairing'
  ]

  const contractNames = verifiers.concat(normalContracts)

  await generateVerifierContractAlt(
    paths.commitmentVkAlt,
    path.join(contractsDir, 'CommitmentVerifier.sol'),
    'CommitmentVerifier'
  )
  await generateVerifierContractAlt(
    paths.additionVkAlt,
    path.join(contractsDir, 'AdditionVerifier.sol'),
    'AdditionVerifier'
  )
  await generateVerifierContractAlt(
    paths.transferVkAlt,
    path.join(contractsDir, 'TransferVerifier.sol'),
    'TransferVerifier'
  )
  await generateVerifierContractAlt(
    paths.withdrawalVkAlt,
    path.join(contractsDir, 'WithdrawalVerifier.sol'),
    'WithdrawalVerifier'
  )

  await Promise.all(normalContracts.map(name => {
    const filename = name + '.sol'
    return asyncFs.copyFile(
      path.join(contractsSrcDir, filename),
      path.join(contractsDir, filename)
    )
  }))

  const outputPath = path.join(contractsDir, 'output.json')
  const command = [
    `solc --combined-json abi,bin openzeppelin-solidity=${ozDir}`,
    '*.sol > output.json'
  ].join(' ')
  const options = {
    cwd: contractsDir,
    maxBuffer: 1024 * 1024
  }
  const { stderr } = await execAsync(command, options)

  if (stderr) {
    console.log(stderr)
  }
  const json = await asyncFs.readTextFile(outputPath)
  await Promise.all(contractNames.map(name => {
    return asyncFs.unlink(path.join(contractsDir, `${name}.sol`))
  }))

  // await asyncFs.unlink(contractsDir)
  const compilationResult = JSON.parse(json)

  return _.zipObject(
    contractNames,
    contractNames.map(contractName => {
      return extractContractArtefacts(compilationResult, contractName)
    })
  )

}

module.exports = compileContracts
