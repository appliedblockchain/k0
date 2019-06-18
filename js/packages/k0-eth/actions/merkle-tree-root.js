const BN = require('bn.js')
const u = require('@appliedblockchain/k0-util')

async function merkleTreeRoot(web3, mvppt, server) {
  const root = await Promise.all([
    mvppt.methods.root(0).call(),
    mvppt.methods.root(1).call()
  ])
  return server.unpack256Bits(
    new BN((root[0] || 0).toString()),
    new BN((root[1] || 0).toString())
  )
}

module.exports = merkleTreeRoot
