const assert = require('assert')
const readAndSplit = require('./read-and-split')
const u = require('./util')

// Reads a verification key from a file.
async function vkFromFile(path) {
  let vk = await readAndSplit(path)
  assert(vk.length >= 24, 'vk too short')
	const A = u.parseG2Point(vk)
	vk = vk.slice(4)
	const B = u.parseG1Point(vk)
	vk = vk.slice(2)
	const C = u.parseG2Point(vk)
	vk = vk.slice(4)
	const gamma = u.parseG2Point(vk)
	vk = vk.slice(4)
	const gammaBeta1 = u.parseG1Point(vk)
	vk = vk.slice(2)
	const gammaBeta2 = u.parseG2Point(vk)
	vk = vk.slice(4)
	const Z = u.parseG2Point(vk)
	vk = vk.slice(4)
  const IC = []
	while(vk != [] && vk[0] != "") {
		IC.push(u.parseG1Point(vk))
		vk = vk.slice(2)
	}
  return [ A, B, C, gamma, gammaBeta1, gammaBeta2, Z, IC ]
}

module.exports = vkFromFile
