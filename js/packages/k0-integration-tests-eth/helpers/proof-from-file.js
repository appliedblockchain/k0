const assert = require('assert')
const readAndSplit = require('./read-and-split')
const u = require('./util')

// Reads a proof from a file.
async function proofFromFile(path) {
  let proof = await readAndSplit(path)
  assert(proof.length >= 18, 'proof too short')
	const A_g = u.parseG1Point(proof)
	proof = proof.slice(2)
	const A_h = u.parseG1Point(proof)
	proof = proof.slice(2)
	const B_g = u.parseG2Point(proof)
	proof = proof.slice(4)
	const B_h = u.parseG1Point(proof)
	proof = proof.slice(2)
	const C_g = u.parseG1Point(proof)
	proof = proof.slice(2)
	const C_h = u.parseG1Point(proof)
	proof = proof.slice(2)
	const H = u.parseG1Point(proof)
	proof = proof.slice(2)
	const K = u.parseG1Point(proof)
	proof = proof.slice(2)
	return [ A_g, A_h, B_g, B_h, C_g, C_h, H, K ]
}

module.exports = proofFromFile
