const expect = require('code').expect
const jayson = require('jayson/promise')
const testUtil = require('./util')

describe('Server', function() {

  let jsonrpc

  before(function() {
    console.log('initing')
    const port = parseInt(process.env.SERVER_PORT || "4000")
    jsonrpc = jayson.client.http({ port })
  })

  it('example proof generation and verification', async function() {

    //  "Vitalik's example": x**3 + x + 5 == out
    const examples = [
      { x: "3", out: "35" },
      { x: "9284982373", out: "800466665197865628030674911495" }
    ]

    const results = []

    for (let i = 0; i < examples.length; i = i + 1) {
      const ex = examples[i]
      const proofRes = await jsonrpc.request('exampleWitnessAndProof', [ex.x])
      expect(proofRes.error).to.be.undefined()
      const out = proofRes.result.out
      expect(out).to.equal(ex.out)
      const proof = proofRes.result.proofJacobian
      results.push({ out, proof })
    }

    const verifyRes1 = await jsonrpc.request(
      'verifyProof',
      [ 'example', results[0].proof, [ examples[1].out ] ]
    )
    expect(verifyRes1.result).to.be.false()

    const verifyRes2 = await jsonrpc.request(
      'verifyProof',
      [ 'example', results[1].proof, [ results[0].out ] ]
    )
    expect(verifyRes2.result).to.be.false()

    const verifyRes3 = await jsonrpc.request(
      'verifyProof',
      [ 'example', results[0].proof, [ results[0].out ] ]
    )
    expect(verifyRes3.result).to.be.true()

    const verifyRes4 = await jsonrpc.request(
      'verifyProof',
      [ 'example', results[1].proof, [ results[1].out ] ]
    )
    expect(verifyRes4.result).to.be.true()
  })

  it("packing and unpacking", async function() {
    const hexVal = testUtil.randomBytesHex(32)
    const packingRes = await jsonrpc.request('pack256Bits', [ hexVal ])
    expect(packingRes.error).to.be.undefined()
    const unpackingRes = await jsonrpc.request(
      'unpack256Bits', packingRes.result
    )
    expect(unpackingRes.error).to.be.undefined()
    expect(unpackingRes.result).to.equal(hexVal)
  })

})
