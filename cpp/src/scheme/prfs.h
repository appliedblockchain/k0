#ifndef ZKTRADE_SCHEME_PRFS_H
#define ZKTRADE_SCHEME_PRFS_H

#include <libff/common/utils.hpp>
using namespace libff;

namespace zktrade {
    template<typename HashT>
    bit_vector prf_sn(bit_vector a_sk, bit_vector rho);
    template<typename HashT>
    bit_vector prf_addr(bit_vector a_sk);
}

#include "prfs.tcc"

#endif //ZKTRADE_SCHEME_PRFS_H
