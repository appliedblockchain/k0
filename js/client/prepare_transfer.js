const request = require('./request')

function prepare_transfer(jc, params) {
  return request(
    jc,
    'prepare_transfer',
    params
  )
}

module.exports = prepare_transfer