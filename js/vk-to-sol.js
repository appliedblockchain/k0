function g2(label, p) {
  return `\t\tvk.${label} = Pairing.G2Point([uint256(${p[0][0]}), uint256(${p[0][1]})], [uint256(${p[1][0]}), uint256(${p[1][1]})]);`
}

function g1(label, p) {
  return `\t\tvk.${label} = Pairing.G1Point(uint256(${p[0]}), uint256(${p[1]}));`
}

function vkToSol(A, B, C, gamma, gammaBeta1, gammaBeta2, Z, IC) {
  const icLines = IC
  return [
    g2('A', A),
    g1('B', B),
    g2('C', C),
    g2('gamma', gamma),
    g1('gammaBeta1', gammaBeta1),
    g2('gammaBeta2', gammaBeta2),
    g2('Z', Z),
		`vk.IC = new Pairing.G1Point[](${IC.length});`,
    ...IC.map((p, idx) => g1(`IC[${idx}]`, p))
  ].join('\n')
}

module.exports = vkToSol
