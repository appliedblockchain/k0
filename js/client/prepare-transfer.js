const BN = require('bn.js')
const request = require('./request')
const u = require('../util')
const conv = require('./proof-conversion')

const ZERO_ADDRESS = '0x0000000000000000000000000000000000000000'

async function prepareTransfer(
  jc,
  prev_root,
  in_0_address,
  in_0_a_sk,
  in_0_rho,
  in_0_r,
  in_0_v,
  in_0_path,
  in_1_address,
  in_1_a_sk,
  in_1_rho,
  in_1_r,
  in_1_v,
  in_1_path,
  out_0_a_pk,
  out_0_rho,
  out_0_r,
  out_0_v,
  out_1_a_pk,
  out_1_rho,
  out_1_r,
  out_1_v,
  calleeAddress
) {

  u.checkBuf(prev_root, 32)

  u.checkBN(in_0_address)
  u.checkBuf(in_0_a_sk, 32)
  u.checkBuf(in_0_rho, 32)
  u.checkBuf(in_0_r, 48)
  u.checkBN(in_0_v)
  in_0_path.forEach(x => u.checkBuf(x, 32))

  u.checkBN(in_1_address)
  u.checkBuf(in_1_a_sk, 32)
  u.checkBuf(in_1_rho, 32)
  u.checkBuf(in_1_r, 48)
  u.checkBN(in_1_v)
  in_1_path.forEach(x => u.checkBuf(x, 32))

  u.checkBuf(out_0_a_pk, 32)
  u.checkBuf(out_0_rho, 32)
  u.checkBuf(out_0_r, 48)
  u.checkBN(out_0_v)

  u.checkBuf(out_1_a_pk, 32)
  u.checkBuf(out_1_rho, 32)
  u.checkBuf(out_1_r, 48)
  u.checkBN(out_1_v)

  if (calleeAddress !== undefined) {
    u.checkBuf(calleeAddress, 20)
  }
  const params = [
    u.buf2hex(prev_root),

    in_0_address,
    u.buf2hex(in_0_a_sk),
    u.buf2hex(in_0_rho),
    u.buf2hex(in_0_r),
    in_0_v.toString(),
    in_0_path.map(x => u.buf2hex(x)),

    in_1_address,
    u.buf2hex(in_1_a_sk),
    u.buf2hex(in_1_rho),
    u.buf2hex(in_1_r),
    in_1_v.toString(),
    in_1_path.map(x => u.buf2hex(x)),

    u.buf2hex(out_0_a_pk),
    u.buf2hex(out_0_rho),
    u.buf2hex(out_0_r),
    out_0_v.toString(),

    u.buf2hex(out_1_a_pk),
    u.buf2hex(out_1_rho),
    u.buf2hex(out_1_r),
    out_1_v.toString(),

    calleeAddress !== undefined ? u.buf2hex(calleeAddress) : ZERO_ADDRESS
  ]

  const res = await request(jc, 'prepareTransfer', params)

  console.log({ res })
  return {
    input_0_sn: u.hex2buf(res.input_0_sn),
    input_1_sn: u.hex2buf(res.input_1_sn),
    output_0_address: new BN(res.output_0_address),
    output_0_cm: u.hex2buf(res.output_0_cm),
    output_1_address: new BN(res.output_1_address),
    output_1_cm: u.hex2buf(res.output_1_cm),
    proofAffine: conv.bnifyAffine(res.proof)
  }
}

module.exports = prepareTransfer
