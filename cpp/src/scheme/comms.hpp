#ifndef ZKTRADE_SCHEME_COMMS_HPP
#define ZKTRADE_SCHEME_COMMS_HPP

#include <libff/common/utils.hpp>

using namespace libff;

namespace zktrade {
    template<typename HashT>
    bit_vector comm_r(bit_vector a_pk, bit_vector rho, bit_vector r);
    template<typename HashT>
    bit_vector comm_s(bit_vector k, bit_vector v);
}

#include "comms.tcc"

#endif //ZKTRADE_SCHEME_COMMS_HPP
