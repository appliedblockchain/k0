const BN = require('bn.js')
const crypto = require('crypto')
const inquirer = require('inquirer')
const u = require('../util')

async function inquireOutputNote(publicKeys, label, maxValue, justUseMaxValue = false) {
  u.checkBN(maxValue)
  const data = {}
  const recipientInquiryResult = await inquirer.prompt([{
    type: 'list',
    name: 'recipient',
    message: `${label}: Recipient`,
    choices: Object.keys(publicKeys)
  }])
  data.a_pk = publicKeys[recipientInquiryResult.recipient]
  if (justUseMaxValue) {
    data.v = maxValue
  } else {
    const vInquiryResult = await inquirer.prompt([{
      type: 'input',
      name: 'v',
      message: `${label}: Value (max: ${maxValue.toString()})`
    }])
    data.v = new BN(vInquiryResult.v)
    assert(data.v.lte(maxValue))
  }
  const rhoInquiryResult = await inquirer.prompt([{
    type: 'input',
    name: 'rho',
    message: `${label}: rho`,
    default: 'random'
  }])
  if (rhoInquiryResult.rho === 'random') {
    data.rho = crypto.randomBytes(32)
  } else {
    data.rho = u.hex2buf(rhoInquiryResult)
    u.checkBuf(data.rho, 32)
  }
  const rInquiryResult = await inquirer.prompt([{
    type: 'input',
    name: 'r',
    message: `${label}: r`,
    default: 'random'
  }])
  if (rInquiryResult.r === 'random') {
    data.r = crypto.randomBytes(48)
  } else {
    data.r = u.hex2buf(rInquiryResult)
    u.checkBuf(data.r, 48)
  }
  return data
}

module.exports = inquireOutputNote
