#ifndef ZKTRADE_INIT_HPP
#define ZKTRADE_INIT_HPP

#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>

namespace zktrade {}

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

#ifdef ZKTRADE_DUMMYHASH

#include "circuitry/gadgets/dummyhash_gadget.hpp"
typedef dummyhash_compression_gadget<FieldT> CommitmentHashT;
typedef dummyhash_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

#else

#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
typedef sha256_compression_gadget<FieldT> CommitmentHashT;
typedef sha256_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

#endif

#endif //ZKTRADE_INIT_HPP
