#include "comms.hpp"
#include "hash/sha256_compression.hpp"
#include "util.h"

using namespace libff;
using namespace zktrade;

bit_vector
zktrade::comm_r(bit_vector a_pk, bit_vector rho, bit_vector r) {
    if (a_pk.size() != 256) {
        throw invalid_argument("Bit vector a_pk is not of length 256");
    }
    if (rho.size() != 256) {
        throw invalid_argument("Bit vector rho is not of length 256");
    }
    if (r.size() != 384) {
        throw invalid_argument("Bit vector r is not of length 384");
    }
    bit_vector comm_a_pk_rho = sha256_compress(concat(a_pk, rho));
    bit_vector comm_truncated = truncate(comm_a_pk_rho, 128);
    return sha256_compress(concat(r, comm_truncated));
}

bit_vector
zktrade::comm_s(bit_vector k, bit_vector v) {
    if (k.size() != 256) {
        throw invalid_argument("Bit vector k is not of length 256");
    }
    if (v.size() != 64) {
        throw invalid_argument("Bit vector v is not of length 64");
    }
    bit_vector zeros = zero_bits(192);
    return sha256_compress(concat(k, zeros, v));
}
