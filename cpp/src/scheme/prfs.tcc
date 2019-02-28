#include "prfs.h"
#include "util.h"

using namespace libff;
using namespace std;
using namespace zktrade;


template<typename HashT>
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
    return HashT::get_hash(concat(x, right_side));
}

template<typename HashT>
bit_vector zktrade::prf_addr(bit_vector a_sk) {
    bit_vector zeros(254);
    std::fill(zeros.begin(), zeros.end(), 0);
    return prf<HashT>(a_sk, {0,0}, zeros);
}

template<typename HashT>
bit_vector zktrade::prf_sn(bit_vector a_sk, bit_vector rho) {
    return prf<HashT>(a_sk, {0,1}, truncate(rho, 254));
}
