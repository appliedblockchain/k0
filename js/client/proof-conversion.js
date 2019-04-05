const BN = require('bn.js')

function affineG1(point) {
  return {
    x: new BN(point.x),
    y: new BN(point.y)
  }
}

function affineG2(point) {
  return {
    x: {
      c0: new BN(point.x.c0),
      c1: new BN(point.x.c1)
    },
    y: {
      c0: new BN(point.y.c0),
      c1: new BN(point.y.c1)
    }
  }
}

function jacobianG1(point) {
  return {
    x: new BN(point.x),
    y: new BN(point.y),
    z: new BN(point.z)
  }
}

function jacobianG2(point) {
  return {
    x: {
      c0: new BN(point.x.c0),
      c1: new BN(point.x.c1)
    },
    y: {
      c0: new BN(point.y.c0),
      c1: new BN(point.y.c1)
    },
    z: {
      c0: new BN(point.z.c0),
      c1: new BN(point.z.c1)
    }
  }
}

function convertProof(g1fnc, g2fnc, input) {
  return {
    g_A: {
      g: g1fnc(input.g_A.g),
      h: g1fnc(input.g_A.h)
    },
    g_B: {
      g: g2fnc(input.g_B.g),
      h: g1fnc(input.g_B.h)
    },
    g_C: {
      g: g1fnc(input.g_C.g),
      h: g1fnc(input.g_C.h)
    },
    g_H: g1fnc(input.g_H),
    g_K: g1fnc(input.g_K)
  }
}

module.exports = {
  convertAffine: convertProof.bind(null, affineG1, affineG2),
  convertJacobian: convertProof.bind(null, jacobianG1, jacobianG2),
}