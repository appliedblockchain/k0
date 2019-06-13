const crypto = require('crypto')
const inquireInputNote = require('../inquire-input-note')
const inquireOutputNote = require('../inquire-output-note')
const inquirer = require('inquirer')
const u = require('@appliedblockchain/k0-util')

function makeData(a_pk, rho, r, v) {
  u.checkBuf(a_pk, 32)
  u.checkBuf(rho, 32)
  u.checkBuf(r, 48)
  u.checkBN(v)
  return Buffer.concat([ a_pk, rho, r, v.toBuffer('le', 64)])
}

async function transferMoney(platformState, secretStore, k0Fabric, k0, publicKeys) {
  const inquireInput = inquireInputNote.bind(null, platformState, secretStore)
  const in0 = await inquireInput('First input note')
  const in1 = await inquireInput('Second input note')
  const totalValue = in0.v.add(in1.v)
  const out0 = await inquireOutputNote(publicKeys, 'First output note', totalValue, false)
  const callee = u.hex2buf('0x0000000000000000000000000000000000000000')
  const out1 = await inquireOutputNote(publicKeys, 'Second output note (change)', totalValue.sub(out0.v), true, false)
  const transferData = await k0.prepareTransfer(platformState, secretStore, in0.address, in1.address, out0, out1, callee)
  secretStore.addSNToNote(in0.cm, transferData.input_0_sn)
  secretStore.addSNToNote(in1.cm, transferData.input_1_sn)
  secretStore.addNoteInfo(transferData.output_0_cm, out0.a_pk, out0.rho, out0.r, out0.v)
  secretStore.addNoteInfo(transferData.output_1_cm, out1.a_pk, out1.rho, out1.r, out1.v)

  const rootBefore = await platformState.merkleTreeRoot()
  const labelBefore = platformState.currentStateLabel()
  const tmpLabel = ('temporary_mt_addition_' + crypto.randomBytes(16).toString('hex'))
  await platformState.add(tmpLabel, [], [ transferData.output_0_cm, transferData.output_1_cm ])
  const newRoot = await platformState.merkleTreeRoot()
  await platformState.rollbackTo(labelBefore)

  const finalRoot = await platformState.merkleTreeRoot()
  assert(finalRoot.equals(rootBefore))

  const out_0_data = makeData(out0.a_pk, out0.rho, out0.r, out0.v)
  const out_1_data = makeData(out1.a_pk, out1.rho, out1.r, out1.v)

  await k0Fabric.transfer(
    u.buf2hex(transferData.input_0_sn),
    u.buf2hex(transferData.input_1_sn),
    u.buf2hex(transferData.output_0_cm),
    u.buf2hex(transferData.output_1_cm),
    u.buf2hex(out_0_data),
    u.buf2hex(out_1_data),
    u.buf2hex(newRoot)
  )
}

module.exports = transferMoney
