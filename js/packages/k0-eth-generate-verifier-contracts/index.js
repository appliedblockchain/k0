const generateVerifierContract = require('./generate-verifier-contract')
const path = require('path')

async function generateVerifierContracts(
  contractsDir,
  commitmentVkPath,
  additionVkPath,
  transferVkPath,
  withdrawalVkPath
) {
  const verifiers = [
    'AdditionVerifier',
    'CommitmentVerifier',
    'WithdrawalVerifier',
    'TransferVerifier'
  ]

  await Promise.all([
    [ 'Commitment', commitmentVkPath ],
    [ 'Addition', additionVkPath ],
    [ 'Transfer', transferVkPath ],
    [ 'Withdrawal', withdrawalVkPath ],
  ].map(pair => {
    const [ name, pathToVk ] = pair
    return generateVerifierContract(
      pathToVk,
      path.join(contractsDir, `${name}Verifier.sol`),
      `${name}Verifier`
    )
  }))
}

module.exports = generateVerifierContracts
