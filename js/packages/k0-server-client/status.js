const request = require('./request')

async function status(jc) {
  return request(jc, 'status')
}

module.exports = status
