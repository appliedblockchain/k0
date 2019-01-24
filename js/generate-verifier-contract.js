'use strict'

const asyncFs = require('./async-fs')
const path = require('path')
const vkFromFile = require('./vk-from-file')
const vkToSol = require('./vk-to-sol')

async function generateVerifierContract(pathToVk, filePath) {
  // Read vk and Solidity contract source code template
  const templateFilePath = path.join(path.parse(module.filename).dir, '..', 'sol', 'Verifier.sol')
  const [ vk, contractTemplate ] = await Promise.all([
    vkFromFile(pathToVk),
    asyncFs.readTextFile(templateFilePath)
  ])

  // Put vk into contract source, write to temporary file
  const vkSolSnippet = vkToSol(...vk)
  const contractSource = contractTemplate.replace('____VERIFYING_KEY_BODY____', vkSolSnippet)
  await asyncFs.writeTextFile(filePath, contractSource, 'utf8')
}

module.exports = generateVerifierContract