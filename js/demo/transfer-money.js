const crypto = require('crypto')
const inquireInputNote = require('./inquire-input-note')
const inquireOutputNote = require('./inquire-output-note')
const u = require('../util')

function makeData(a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  return Buffer.concat([ a_pk, rho, r, v.toBuffer('le', 64)])
}

async function transferMoney(web3, platformState, secretStore, k0Eth, k0, publicKeys) {
  console.log('Transfer: initial root: ', await platformState.merkleTreeRoot())
  const inquireInput = inquireInputNote.bind(null, platformState, secretStore)
  const in0 = await inquireInput('First input note')
  const in1 = await inquireInput('Second input note')
  const totalValue = in0.v.add(in1.v)
  const out0 = await inquireOutputNote(publicKeys, 'First output note', totalValue)
  const out1 = await inquireOutputNote(publicKeys, 'Second output note', totalValue.sub(out0.v), true)
  const transferData = await k0.prepareTransfer(platformState, secretStore, in0.address, in1.address, out0, out1)
  secretStore.addSNToNote(in0.cm, transferData.input_0_sn)
  secretStore.addSNToNote(in1.cm, transferData.input_1_sn)
  secretStore.addNoteInfo(transferData.output_0_cm, out0.a_pk, out0.rho, out0.r, out0.v)
  secretStore.addNoteInfo(transferData.output_1_cm, out1.a_pk, out1.rho, out1.r, out1.v)

  const rootBefore = await platformState.merkleTreeRoot()
  console.log('Transfer: root after prepareTransfer (before temp addition): ', rootBefore)
  const labelBefore = platformState.currentStateLabel()
  const tmpLabel = ('temporary_mt_addition_' + crypto.randomBytes(16).toString('hex'))
  await platformState.add(tmpLabel, [], [ transferData.output_0_cm, transferData.output_1_cm ])
  const newRoot = await platformState.merkleTreeRoot()
  console.log('Transfer: root after temp addition: ', newRoot)
  await platformState.rollbackTo(labelBefore)

  const finalRoot = await platformState.merkleTreeRoot()
  console.log('Transfer: root after rollback: ', finalRoot)
  assert(finalRoot.equals(rootBefore))

  const out_0_data = makeData(out0.a_pk, out0.rho, out0.r, out0.v)
  const out_1_data = makeData(out1.a_pk, out1.rho, out1.r, out1.v)

  const ethParams = [
    crypto.randomBytes(32),
    transferData.input_0_sn,
    transferData.input_1_sn,
    transferData.output_0_cm,
    transferData.output_1_cm,
    out_0_data,
    out_1_data,
    newRoot,
    u.hex2buf('0x0000000000000000000000000000000000000000'),
    transferData.proof
  ]
  const tx = await k0Eth.transfer(...ethParams)

  const receipt = await web3.eth.sendSignedTransaction(u.buf2hex(tx))
}

module.exports = transferMoney
