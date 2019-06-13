const u = require('@appliedblockchain/k0-util')

async function merkleTreeRoot(web3, mvppt) {
  const root = await Promise.all([
    mvppt.methods.root(0).call(),
    mvppt.methods.root(1).call()
  ])
  console.log(root)
  return u.unpack(root)
}

module.exports = merkleTreeRoot
