const chalk = require('chalk')
const log = console.log
const Table = require('cli-table')

async function printState(k0State, carToken, accountAddresses, accountNames, carIds) {
  log()
  log(chalk.underline('K0Cash'))
  const k0Table = new Table({
    head: ['State entries'].map(x => chalk.cyan(x))
  });
  k0Table.push(['...'])
  k0State.slice(-6).forEach(row => k0Table.push([row]))

  log(k0Table.toString())

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
