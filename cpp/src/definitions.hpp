#ifndef K0_INIT_HPP
#define K0_INIT_HPP

#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>

namespace k0 {}

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace k0;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

#ifdef K0_DUMMYHASH

#include "circuitry/gadgets/dummyhash_gadget.hpp"
typedef dummyhash_compression_gadget<FieldT> CommitmentHashT;
typedef dummyhash_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

#else

#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "circuitry/gadgets/sha256_compression.h"
typedef sha256_compression_gadget<FieldT> CommitmentHashT;
typedef sha256_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

#endif

#endif //K0_INIT_HPP
