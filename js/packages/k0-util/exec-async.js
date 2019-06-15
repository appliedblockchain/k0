'use strict'
const { exec } = require('child_process')

async function execAsync(cmd, options = null) {
  options = Object.assign({ maxBuffer: 1024 * 1024 }, options)
  return new Promise(function (resolve, reject) {
    exec(cmd, options, (err, stdout, stderr) => {
      if (err) {
        reject(err)
      } else {
        resolve({ stdout, stderr })
      }
    })
  })
}

module.exports = execAsync
