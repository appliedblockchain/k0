#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
using namespace libsnark;

#include "printbits.hpp"
#include "serialization.hpp"
#include "circuits/NoteAdditionCircuit.hpp"
#include "circuits/NoteInclusionCircuit.hpp"
#include "util.hpp"
#include "MerkleTree.hpp"

using namespace std;
using namespace libff;
using namespace libsnark;

int main()
{
    typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
    typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
    default_r1cs_ppzksnark_pp::init_public_params();
    return 0;
}

