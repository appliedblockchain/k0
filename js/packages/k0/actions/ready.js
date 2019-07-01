async function ready(server) {
  const status = await server.status()
  return status.ready
}

module.exports = ready
