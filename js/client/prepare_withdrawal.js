const request = require('./request')

function prepare_withdrawal(jc, address, a_sk, rho, r, v, recipient) {
  return request(
    jc,
    'prepare_withdrawal',
    [
      address,
      a_sk,
      rho,
      r,
      v,
      recipient
    ]
  )
}

module.exports = prepare_withdrawal