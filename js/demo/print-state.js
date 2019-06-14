const chalk = require('chalk')
const log = console.log
const Table = require('cli-table')
const u = require('../util')
const inquirer = require('inquirer')

async function printState(secretStore, ethAddressBook, mvpptAddressBook, platform, platformState, carToken, carIds) {
  carIds.forEach(u.checkBN)
  log(chalk.underline('Notes'))
  const notesTable = new Table({
    head: ['ID', 'CM', 'Owner', 'Value', 'SN'].map(x => chalk.cyan(x)),
    colAligns: ['right', 'left', 'left', 'right', 'left']
  });
  const state = platformState.currentState()
  const ourPublicKey = secretStore.getAPk()
  for (let i = 0; i < state.cmList.length; i++) {
    const cm = state.cmList[i]
    const info = secretStore.getNoteInfo(cm)
    let owner = ''
    if (info !== null) {
      let ownerName
      if (info.a_pk.equals(ourPublicKey)) {
        ownerName = 'us'
      } else {
        ownerName = mvpptAddressBook(info.a_pk) || 'unknown'
      }
      owner = `${ownerName} (${u.shorthex(info.a_pk)})`
    }
    let cells = [
      i,
      u.shorthex(cm),
      owner,
      info !== null ? info.v.toString() : '',
      info !== null && info.sn ? u.shorthex(info.sn) : '',
    ]
    if (info !== null && info.sn) {
      cells = cells.map(cell => chalk.grey(chalk.strikethrough(cell)))
    } else if (info !== null && info.a_pk.equals(ourPublicKey)) {
      cells = cells.map(cell => chalk.cyan(cell))
    }
    notesTable.push(cells)
  }
  // await Promise.all(
  //   accountAddresses.map(async address => {
  //     const name = accountNames[address] || 'unknown'
  //     const balance = await dollarCoin.methods.balanceOf(address).call()
  //     return [`${name} (${address})`, balance]
  //   })
  // )
  // dollarTableData.forEach(row => dollarTable.push(row))

  log()
  log(notesTable.toString())

  log(chalk.underline('CarToken'))

  const carTable = new Table({
    head: ['Car ID', 'Owner'].map(x => chalk.cyan(x)),
    colAligns: ['right', 'left']
  })
  await Promise.all(
    carIds.map(async id => {
      const address = u.hex2buf(await carToken.methods.ownerOf(id.toString()).call())
      const name = ethAddressBook(address) || 'unknown'
      carTable.push([id, `${name} (${u.buf2hex(address)})`])
    })
  )
  log(carTable.toString())

}

module.exports = printState
