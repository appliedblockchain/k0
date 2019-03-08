const request = require('./request')

module.exports = (jc, cm) => request(jc, 'add', [ cm ])
