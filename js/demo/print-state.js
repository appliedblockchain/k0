const chalk = require('chalk')
const log = console.log
const Table = require('cli-table')
const u = require('../util')

async function printState(secretStore, platformState) {
  log()
  log(chalk.underline('Notes'))
  const notesTable = new Table({
    head: ['ID', 'CM', 'Owner', 'Value', 'SN'].map(x => chalk.cyan(x)),
    colAligns: ['right', 'left', 'left', 'right', 'left']
  });
  const state = platformState.currentState()
  const ourPublicKey = secretStore.getPublicKey()
  for (let i = 0; i < state.cmList.length; i++) {
    const cm = state.cmList[i]
    const info = secretStore.getNoteInfo(cm)
    let owner = ''
    if (info !== null) {
      let ownerName
      console.log(info.a_pk, ourPublicKey)
      if (info.a_pk.equals(ourPublicKey)) {
        ownerName = 'us'
      } else {
        ownerName = 'someone'
      }
      owner = `${ownerName} (${u.shorthex(info.a_pk)})`
    }
    notesTable.push([
      i,
      u.shorthex(cm),
      owner,
      info === null ? '' : info.v.toString(),
      ''
    ])
  }
  // await Promise.all(
  //   accountAddresses.map(async address => {
  //     const name = accountNames[address] || 'unknown'
  //     const balance = await dollarCoin.methods.balanceOf(address).call()
  //     return [`${name} (${address})`, balance]
  //   })
  // )
  // dollarTableData.forEach(row => dollarTable.push(row))

  log(notesTable.toString())

  // log(chalk.underline('CarToken'))

  // const carTable = new Table({
  //   head: ['Car ID', 'Owner'].map(x => chalk.cyan(x)),
  //   colAligns: ['right', 'left']
  // })
  // await Promise.all(
  //   carIds.map(async id => {
  //     const address = await carToken.methods.ownerOf(id).call()
  //     const name = accountNames[address] || 'unknown'
  //     carTable.push([id, `${name} (${address})`])
  //   })
  // )
  // log(carTable.toString())
  log()
}

module.exports = printState
