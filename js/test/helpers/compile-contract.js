const path = require('path')
const execAsync = require('../../exec-async')

async function compileContract(contractName) {
  const ozDir = path.join(
    __dirname,
    '..',
    '..',
    'node_modules',
    'openzeppelin-solidity'
  )
  const contractPath = path.join(
    __dirname,
    '..',
    '..',
    '..',
    'sol',
    `${contractName}.sol`
  )
  const command = [
    `solc --combined-json abi,bin openzeppelin-solidity=${ozDir} `,
    `${contractPath}`
  ].join('')
  console.log(command)
  const { stdout, stderr } = await execAsync(command)
  if (stderr) {
    console.log(stderr)
  }
  const result = JSON.parse(stdout)
  const contractInfo = result.contracts[`${contractPath}:${contractName}`]
  return {
    abi: JSON.parse(contractInfo.abi),
    bytecode: '0x' + contractInfo.bin
  }
}

module.exports = compileContract