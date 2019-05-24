const BN = require('bn.js')
const inquirer = require('inquirer')

async function inquireInputNote(platformState, secretStore, message) {
  const inquiryResult = await inquirer.prompt([
    { type: 'number', name: 'noteId', message }
  ])
  const address = new BN(inquiryResult.noteId)
  const cm = platformState.cmAtIndex(address)
  const info = secretStore.getNoteInfo(cm)

  return {
    address,
    cm,
    ...info
  }
}

module.exports = inquireInputNote
