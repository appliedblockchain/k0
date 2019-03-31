const _ = require('lodash')

async function request(jc, methodName, params = []) {
  const response = await jc.request(methodName, params)
  if (!response.result) {
    throw new Error([
      methodName,
      ' errored: ',
      _.get(response, 'error.message', 'Error from server'),
      ` (params: ${params.join(', ')})`
    ].join(''))
  }
  return response.result
}

module.exports = request
