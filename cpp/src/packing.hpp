#include <libff/common/utils.hpp>

using namespace libff;
using namespace libsnark;
using namespace std;

template<typename FieldT>
vector<FieldT> pack(bit_vector bv);

template<typename FieldT>
bit_vector unpack(vector<FieldT> fev);

#include "packing.tcc"