const request = require('./request')

function a_pk(jc, a_sk) {
  return request(jc, 'prf_addr', [ a_sk ])
}

module.exports = a_pk
