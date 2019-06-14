'use strict'

const crypto = require('crypto')
const makeClient = require('../../client')

describe('JSONRPC client', function() {

  let client

  before(function() {
    client = makeClient(parseInt(process.env.SERVER_PORT || "4000"))
  })

  describe('Encryption and decryption', function() {
    it('works', async function() {
      const bob_a_sk = crypto.randomBytes(32)
      const bobKeys = await client.deriveKeys(bob_a_sk)
      const plaintext = crypto.randomBytes(88)
      const ciphertext = await client.encryptNote(plaintext, bobKeys.pk_enc)
      const bobDecrRes = await client.decryptNote(ciphertext, bobKeys.sk_enc,
                                                 bobKeys.pk_enc)
      assert(bobDecrRes.success)
      assert(bobDecrRes.value && bobDecrRes.value.equals(plaintext))
      const eve_a_sk = crypto.randomBytes(32)
      const eveKeys = await client.deriveKeys(eve_a_sk)
      const eveDecrRes = await client.decryptNote(ciphertext, eveKeys.sk_enc,
                                                 eveKeys.pk_enc)
      assert(eveDecrRes.success === false)
    })
  })
})
