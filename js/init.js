
const tmpDir = process.env.TMP_DIR || '/tmp'

async function init() {
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
  return { web3, addresses, vk, proof, contractTemplate }
}

