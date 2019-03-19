const flattenProof = require('./flatten-proof')

async function deposit(web3, server, keys, mvppt, amount) {
  assert(BN.isBN(amount))
  const rho = crypto.randomBytes(32)
  const r = crypto.randomBytes(32)
  const data = await server.prepare_deposit(keys.a_pk, c.rho, c.r, c.v.toString())
  const commitmentProofCompact = flattenProof(data.commitmentProof)
  const additionProofCompact = flattenProof(data.additionProof)

  const params = [
    c.v.toString(),
    await util.pack256Bits(data.k),
    await util.pack256Bits(data.cm),
    await util.pack256Bits(data.nextRoot),
    commitmentProofCompact,
    additionProofCompact
  ]
  const receipt = await sendTransaction(
    web3,
    mvppt._address,
    mvppt.methods.deposit(...params).encodeABI(),
    5000000,
    accounts.bob
  )

  assert(receipt.status)

  server.add(data.cm)
}
