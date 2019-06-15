'use strict'

const BN = require('bn.js')
const crypto = require('crypto')
const inquirer = require('inquirer')
const u = require('@appliedblockchain/k0-util')
const assert = require('assert')

async function inquireOutputNote(
  publicKeys,
  label,
  maxValue,
  justUseMaxValue = false,
  smartPayment = false
) {
  u.checkBN(maxValue)
  const data = {}
  const recipientInquiryResult = await inquirer.prompt([ {
    type: 'list',
    name: 'recipient',
    message: `${label}: Recipient`,
    choices: Object.keys(publicKeys)
  } ])

  data.a_pk = publicKeys[recipientInquiryResult.recipient]
  if (justUseMaxValue) {
    data.v = maxValue
  } else {
    const vInquiryResult = await inquirer.prompt([ {
      type: 'input',
      name: 'v',
      message: `${label}: Value (max: ${maxValue.toString()}, leave empty to send max.)`
    } ])

    if (data.v === '') {
      data.v = new BN(maxValue)
    }

    data.v = new BN(vInquiryResult.v)
    assert(data.v.lte(maxValue))
  }
  if (smartPayment) {
    const rhoInquiryResult = await inquirer.prompt([ {
      type: 'input',
      name: 'rho',
      message: `${label}: rho`
    } ])
    data.rho = u.hex2buf(rhoInquiryResult.rho)
    u.checkBuf(data.rho, 32)
    const rInquiryResult = await inquirer.prompt([ {
      type: 'input',
      name: 'r',
      message: `${label}: r`
    } ])
    data.r = u.hex2buf(rInquiryResult.r)
    u.checkBuf(data.r, 48)
  } else {
    data.rho = crypto.randomBytes(32)
    data.r = crypto.randomBytes(48)
  }
  return data
}

module.exports = inquireOutputNote
