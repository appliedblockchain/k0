const BN = require('bn.js')
const inquirer = require('inquirer')
const u = require('../util')

async function generatePaymentData(secretStore, k0) {
  const inquiryResult = await inquirer.prompt([
    { type: 'number', name: 'v', message: 'Amount' }
  ])
  const v = new BN(inquiryResult.v)
  const { rho, r, cm } = await k0.generatePaymentData(secretStore, v)
  console.log(`  rho: ${u.buf2hex(rho)}`)
  console.log(`  r:   ${u.buf2hex(r)}`)
  console.log(`  v:   ${v.toString()}`)
  console.log(`  cm:  ${u.buf2hex(cm)}`)
}

module.exports = generatePaymentData
