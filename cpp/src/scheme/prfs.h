#ifndef K0_SCHEME_PRFS_H
#define K0_SCHEME_PRFS_H

#include <libff/common/utils.hpp>
using namespace libff;

namespace zktrade {
    // TODO All these deviate a bit from Zerocash/ZCash. Verify if ok
    template<typename HashT>
    bit_vector prf_addr_a_pk(bit_vector a_sk);
    template<typename HashT>
    bit_vector prf_addr_sk_enc(bit_vector a_sk);
    template<typename HashT>
    bit_vector prf_sn(bit_vector a_sk, bit_vector rho);
}

#include "prfs.tcc"

#endif //K0_SCHEME_PRFS_H
