async function get(channel, chaincodeId, queryPeer) {
  const [ buf ] = await channel.queryByChaincode({
    chaincodeId,
    fcn: 'get',
		args: [],
    targets: [ queryPeer ]
	})
  return buf
}

module.exports = get
