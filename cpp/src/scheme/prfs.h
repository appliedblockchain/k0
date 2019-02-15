#ifndef ZKTRADE_SCHEME_PRFS_H
#define ZKTRADE_SCHEME_PRFS_H

#include <libff/common/utils.hpp>
using namespace libff;

namespace zktrade {
    bit_vector prf_sn(bit_vector a_sk, bit_vector rho);
    bit_vector prf_addr(bit_vector a_sk);
}

#endif //ZKTRADE_SCHEME_PRFS_H
