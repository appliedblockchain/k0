#ifndef K0_DATA_HPP
#define K0_DATA_HPP

using namespace std;
using namespace libff;

namespace zktrade {
    struct input_note {
        uint64_t address;
        bit_vector a_sk;
        bit_vector rho;
        bit_vector r;
        uint64_t v;
        vector<bit_vector> path;
    };

    struct output_note {
        bit_vector a_pk;
        bit_vector rho;
        bit_vector r;
        uint64_t v;
    };

}

#endif //K0_DATA_HPP
