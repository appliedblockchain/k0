#ifndef ZKTRADE_SCHEME_COMMS_HPP
#define ZKTRADE_SCHEME_COMMS_HPP

#include <libff/common/utils.hpp>

using namespace libff;

namespace zktrade {
    bit_vector comm_r(bit_vector a_pk, bit_vector rho, bit_vector r);
    bit_vector comm_s(bit_vector k, bit_vector v);
}

#endif //ZKTRADE_SCHEME_COMMS_HPP
