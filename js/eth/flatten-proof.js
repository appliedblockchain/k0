function flattenProof(proof) {
  return [
    proof[0][0], // 0 a
    proof[0][1], // 1

    proof[1][0], // 2 a_p
    proof[1][1], // 3

    proof[2][0][0], // 4 b (0)
    proof[2][0][1], // 5
    proof[2][1][0], // 6 b (1)
    proof[2][1][1], // 7

    proof[3][0], // 8 b_p
    proof[3][1], // 9

    proof[4][0], // 10 c
    proof[4][1], // 11

    proof[5][0], // 12 c_p
    proof[5][1], // 13

    proof[6][0], // 14 h
    proof[6][1], // 15

    proof[7][0], // 16 k
    proof[7][1]  // 17
  ]
}

module.exports = flattenProof