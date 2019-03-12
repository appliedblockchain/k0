const request = require('./request')

function cm(jc, a_pk, rho, r, v) {
  return request(jc, 'cm', [ a_pk, rho, r, v ])
}

module.exports = cm
