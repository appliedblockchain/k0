#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include "../../MerkleTree.hpp"
#include "../../util.hpp"
#include "InclusionCircuit.hpp"

int main()
{
  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
  typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
  default_r1cs_ppzksnark_pp::init_public_params();

  const size_t digest_len = HashT::get_digest_len();

  uint tree_height = 3;

  InclusionCircuit<FieldT, HashT> circuit = make_inclusion_circuit<FieldT, HashT>(tree_height);

  MerkleTree mt{tree_height};

  for (int i = 0; i < 8; i++)
  {
    cout << i << endl;
    libff::bit_vector leaf_bv = random_bits();
    uint address = mt.add(leaf_bv);
    const libff::bit_vector address_bits = libff::convert_field_element_to_bit_vector<FieldT>(FieldT(address, true), tree_height);

    auto root_bv = mt.root();
    auto root_field_elems = field_elements_from_bits<FieldT>(root_bv);
    circuit.root_va->fill_with_field_elements(*circuit.pb, root_field_elems);

    circuit.root_unpacker->generate_r1cs_witness_from_packed();
    circuit.address_bits_va->fill_with_bits(*circuit.pb, address_bits);
    circuit.leaf_digest->generate_r1cs_witness(leaf_bv);
    circuit.path_var->generate_r1cs_witness(address, mt.path(address));

    circuit.mtcrg->generate_r1cs_witness();

    if (circuit.pb->is_satisfied())
    {
      cout << "SATISFIED" << endl;
    }
    else
    {
      cout << "NOT SATISFIED" << endl;
    }
  }
}