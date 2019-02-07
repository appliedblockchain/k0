#ifndef ZKTRADE_PRFS_H
#define ZKTRADE_PRFS_H

#include <libff/common/utils.hpp>

namespace zktrade {
    libff::bit_vector prf_sn(libff::bit_vector x, libff::bit_vector z);
}
#endif //ZKTRADE_PRFS_H
