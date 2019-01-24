#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include "InclusionCircuit.hpp"
#include "../../MerkleTree.hpp"
#include "../../serialization.hpp"
#include "../../util.hpp"

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
    libff::bit_vector r_bv = random_bits();
    libff::bit_vector sn_bv = random_bits();

    libff::bit_vector block = r_bv;
    block.insert(block.end(), sn_bv.begin(), sn_bv.end());
    libff::bit_vector leaf_bv = HashT::get_hash(block);
 
    uint address = mt.add(leaf_bv);
    const libff::bit_vector address_bits = libff::convert_field_element_to_bit_vector<FieldT>(FieldT(address, true), tree_height);

    auto root_bv = mt.root();
    auto root_field_elems = field_elements_from_bits<FieldT>(root_bv);
    circuit.root_va->fill_with_field_elements(*circuit.pb, root_field_elems);

    auto sn_field_elems = field_elements_from_bits<FieldT>(sn_bv);
    circuit.sn_va->fill_with_field_elements(*circuit.pb, sn_field_elems);

    circuit.root_unpacker->generate_r1cs_witness_from_packed();
    circuit.sn_unpacker->generate_r1cs_witness_from_packed();
    circuit.r_digest->generate_r1cs_witness(r_bv);
    circuit.leaf_hasher->generate_r1cs_witness();
    cout << bits2hex(leaf_bv) << endl;
    cout << bits2hex(circuit.leaf_digest->get_digest()) << endl;
    circuit.address_bits_va->fill_with_bits(*circuit.pb, address_bits);
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