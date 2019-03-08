const request = require('./request')

function prepare_deposit(jc, a_pk, rho, r, v) {
  return request(
    jc,
    'prepare_deposit',
    [
      a_pk,
      rho,
      r,
      v
    ]
  )
}

module.exports = prepare_deposit