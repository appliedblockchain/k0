#include "hash/sha256_compression.h"
#include "prfs.h"
#include "util.h"

using namespace libff;
using namespace std;
using namespace zktrade;

bit_vector prf(bit_vector x, bit_vector y, bit_vector z) {
    if (x.size() != 256) {
        throw invalid_argument("Bit vector x is not of length 256");
    }
    if (y.size() != 2) {
        throw invalid_argument("Bit vector y is not of length 2");
    }
    if (z.size() != 254) {
        throw invalid_argument("Bit vector z is not of length 254");
    }
    bit_vector right_side = concat(y, z);
    return sha256_compress(concat(x, right_side));
}

bit_vector zktrade::prf_sn(bit_vector x, bit_vector z) {
    return prf(x, {0,1}, z);
}
