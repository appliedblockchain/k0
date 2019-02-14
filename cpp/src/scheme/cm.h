#ifndef ZKTRADE_SCHEME_CM_H
#define ZKTRADE_SCHEME_CM_H

#include <libff/common/utils.hpp>

using namespace libff;

namespace zktrade {
    bit_vector cm(bit_vector a_pk, bit_vector rho, bit_vector r, bit_vector v);
}

#endif //ZKTRADE_SCHEME_CM_H
