'use strict'
const fs = require('fs')

function readFile(filePath) {
  return Buffer.from(fs.readFileSync(filePath)).toString()
}

module.exports = readFile
