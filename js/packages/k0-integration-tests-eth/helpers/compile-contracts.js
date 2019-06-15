'use strict'

const _ = require('lodash')
const crypto = require('crypto')
const execAsync = require('./exec-async')
const generateVerifierContracts = require('@appliedblockchain/k0-eth-generate-verifier-contracts')
const path = require('path')
const asyncFs = require('./async-fs')

const k0ContractsSrcDir = path.join(
  path.parse(module.filename).dir,
  '..',
  'node_modules',
  '@appliedblockchain',
  'k0-eth-contracts'
)

const testContractsSrcDir = path.join(
  path.parse(module.filename).dir,
  '..',
  'contracts'
)

// Path to openzeppelin contracts
const ozDir = path.join(
  __dirname,
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

  const normalK0Contracts = [
    'Callee',
    'MVPPT',
    'Pairing'
  ]
  let contractNames = verifiers.concat(normalK0Contracts)

  const testContracts = [
    'CarToken',
    'DollarCoin',
    'HiddenPriceCarTrade',
    'MoneyShower'
  ]

  contractNames = contractNames.concat(testContracts)

  await generateVerifierContracts(
    contractsDir,
    paths.commitmentVkAlt,
    paths.additionVkAlt,
    paths.transferVkAlt,
    paths.withdrawalVkAlt
  )

  await Promise.all(normalK0Contracts.map(name => {
    const filename = name + '.sol'
    return asyncFs.copyFile(
      path.join(k0ContractsSrcDir, filename),
      path.join(contractsDir, filename)
    )
  }))

  await Promise.all(testContracts.map(name => {
    const filename = name + '.sol'
    return asyncFs.copyFile(
      path.join(testContractsSrcDir, filename),
      path.join(contractsDir, filename)
    )
  }))

  console.log(contractsDir)
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
