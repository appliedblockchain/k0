const chalk = require('chalk')
const log = console.log
const Table = require('cli-table')

async function printState(dollarCoin, carToken, accounts, accountNames, carIds) {
  log()
  log(chalk.underline('DollarCoin'))
  const dollarTable = new Table({
    head: ['Account', 'Balance'].map(x => chalk.cyan(x)),
    colAligns: ['left', 'right']
  });
  const dollarTableData = await Promise.all(
    Object.keys(accounts).map(async identifier => {
      const address = accounts[identifier].address
      const name = accountNames[address] || 'unknown'
      const balance = await dollarCoin.methods.balanceOf(address).call()
      return [`${name} (${address})`, balance]
    })
  )
  dollarTableData.forEach(row => dollarTable.push(row))

  log(dollarTable.toString())

  log(chalk.underline('CarToken'))

  const carTable = new Table({
    head: ['Car ID', 'Owner'].map(x => chalk.cyan(x)),
    colAligns: ['right', 'left']
  })
  await Promise.all(
    carIds.map(async id => {
      const address = await carToken.methods.ownerOf(id).call()
      const name = accountNames[address] || 'unknown'
      carTable.push([id, `${name} (${address})`])
    })
  )
  log(carTable.toString())
  log()
}

module.exports = printState
