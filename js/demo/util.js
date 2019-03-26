const inquirer = require('inquirer')

async function prompt() {
  console.log()
  const response = await inquirer.prompt({
    type: 'confirm',
    name: 'continue',
    message: 'Continue?'
  })
  if (!response.continue) {
    process.exit(0)
  }
  console.log()
}

module.exports = {
  prompt
}