function encryptNote(server, address, v, rho, r) {
  const pk_enc = address.slice(32)
  // Similar to ZCash spec 5.5
  const notePlaintext = Buffer.concat([
    v.toBuffer('le', 8),
    rho,
    r
  ])
  return server.encryptNote(notePlaintext, pk_enc)
}

module.exports = encryptNote
