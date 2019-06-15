const asyncFs = require('./async-fs')

async function readAndSplit(path) {
  const data = await asyncFs.readTextFile(path)
  return data.replace(/\n/g, " ").split(" ");
}

module.exports = readAndSplit
