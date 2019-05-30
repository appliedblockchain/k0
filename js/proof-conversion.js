'use strict'

const u = require('./util')

function convertG1Affine(convert, point) {
  return {
    x: convert(point.x),
    y: convert(point.y)
  }
}

const bnifyG1Affine = convertG1Affine.bind(null, u.string2bn)
const stringifyG1Affine = convertG1Affine.bind(null, u.bn2string)

function convertG2Affine(convert, point) {
  return {
    x: {
      c0: convert(point.x.c0),
      c1: convert(point.x.c1)
    },
    y: {
      c0: convert(point.y.c0),
      c1: convert(point.y.c1)
    }
  }
}

const bnifyG2Affine = convertG2Affine.bind(null, u.string2bn)
const stringifyG2Affine = convertG2Affine.bind(null, u.bn2string)

function convertG1Jacobian(convert, point) {
  return {
    x: convert(point.x),
    y: convert(point.y),
    z: convert(point.z)
  }
}

const bnifyG1Jacobian = convertG1Jacobian.bind(null, u.string2bn)
const stringifyG1Jacobian = convertG1Jacobian.bind(null, u.bn2string)

function convertG2Jacobian(convert, point) {
  return {
    x: {
      c0: convert(point.x.c0),
      c1: convert(point.x.c1)
    },
    y: {
      c0: convert(point.y.c0),
      c1: convert(point.y.c1)
    },
    z: {
      c0: convert(point.z.c0),
      c1: convert(point.z.c1)
    }
  }
}
const bnifyG2Jacobian = convertG2Jacobian.bind(null, u.string2bn)
const stringifyG2Jacobian = convertG2Jacobian.bind(null, u.bn2string)


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
  bnifyAffine: convertProof.bind(null, bnifyG1Affine, bnifyG2Affine),
  bnifyJacobian: convertProof.bind(null, bnifyG1Jacobian, bnifyG2Jacobian),
  stringifyAffine: convertProof.bind(
    null,
    stringifyG1Affine,
    stringifyG2Affine
  ),
  stringifyJacobian: convertProof.bind(
    null,
    stringifyG1Jacobian,
    stringifyG2Jacobian
  )
}
