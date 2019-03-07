const _ = require('lodash')
const util = require('./util')

describe('Address to int conversion', function() {
  let web3, contract
  before(async function() {
    web3 = util.initWeb3()
  })
  it('does something', async function() {
    console.log('hello', util)
    const account = web3.eth.accounts.create()
    const addresses = [
      '0x0000000000000000000000000000000000000001',
      '0xffffffffffffffffffffffffffffffffffffffff',
      '0xdf1a27Fc2b2EA68525E3dcc5780CbcbE73e6778A',
      '0x61201b8cBA5CAB5c99935146Cd9c6B99050BC47f',
      '0x43658D5cce59e1A63Ad40630c89423Aa23c4E134',
      '0x7a2dAf3028b903c73eAeF361E20aEBbe4190A1c7',
      '0x4C4eDd3Cc65e6bcA86e01c406252A34b7574b835',
      '0x6Ad5182Eb11A9792B1d74833F64f0FEeB7CA72Be',
      '0xCfF76aF5bFC34dbDAa810ed9a2E9edAb093E3472',
      '0xEeDA3e1669579C04A05E01c50aFA77EDB1c061AA',
      '0xE94Ae3289bE44A946d55e77827fF19F44450cA5b',
      '0x105C92C07f47709aa9926c697DED50e56E3E6dF2' ]
    contract = await util.deployStandardContract(web3, 'AddressConversionCheck')
    // const moneyShower = await deployStandardContract(web3, 'MoneyShower')
    console.log(account.address)
    console.log(await Promise.all(addresses.map(address => contract.methods.test(address).call())))
  })
})