function flattenProof(proof) {
  return [
    proof.g_A.g.x, // 0 a
    proof.g_A.g.y, // 1

    proof.g_A.h.x, // 2 a_p
    proof.g_A.h.y, // 3

    proof.g_B.g.x.c0, // 4 b (0)
    proof.g_B.g.x.c1, // 5
    proof.g_B.g.y.c0, // 6 b (1)
    proof.g_B.g.y.c1, // 7

    proof.g_B.h.x, // 8 b_p
    proof.g_B.h.y, // 9

    proof.g_C.g.x, // 10 c
    proof.g_C.h.y, // 11

    proof.g_C.h.x, // 12 c_p
    proof.g_C.h.y, // 13

    proof.g_H.x, // 14 h
    proof.g_H.y, // 15

    proof.g_K.x, // 16 k
    proof.g_K.y  // 17
  ]
}

module.exports = flattenProof