#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "MTLeafAddition.hpp"
#include "../../MerkleTree.hpp"
#include "../../serialization.hpp"
#include "../../util.hpp"

using namespace libsnark;

int main()
{
  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
  typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
  default_r1cs_ppzksnark_pp::init_public_params();

  const size_t digest_len = HashT::get_digest_len();

  for (size_t tree_height = 1; tree_height < 4; tree_height++)
  {
    cout << "TREE HEIGHT " << tree_height << endl;
    protoboard<FieldT> pb;

    pb_variable_array<FieldT> address_bits_va;
    digest_variable<FieldT> prev_root_digest(pb, digest_len, "prev_root");
    digest_variable<FieldT> leaf_digest(pb, digest_len, "leaf");
    digest_variable<FieldT> next_root_digest(pb, digest_len, "next_root");
    merkle_authentication_path_variable<FieldT, HashT> path_var(pb, tree_height, "merkle_authentication_path");

    address_bits_va.allocate(pb, tree_height, "address_bits");

    pb.set_input_sizes(tree_height + 3 * 256);

    MTLeafAddition<FieldT, HashT> g(pb,
                                    tree_height,
                                    address_bits_va,
                                    prev_root_digest,
                                    leaf_digest,
                                    next_root_digest,
                                    path_var);
    g.generate_r1cs_constraints();

    MerkleTree mt(tree_height);

    for (uint address = 0; address < exp2(tree_height); address++)
    {
      const libff::bit_vector address_bits = libff::convert_field_element_to_bit_vector<FieldT>(FieldT(address, true), tree_height);
      libff::bit_vector leaf_bv = random_bits();
      auto simulation_result = mt.simulate_add(leaf_bv);
      libff::bit_vector next_root_bv = get<1>(simulation_result);
      vector<libff::bit_vector> path = get<2>(simulation_result);

      address_bits_va.fill_with_bits(pb, address_bits);
      prev_root_digest.generate_r1cs_witness(mt.root());
      leaf_digest.generate_r1cs_witness(leaf_bv);
      next_root_digest.generate_r1cs_witness(next_root_bv);
      path_var.generate_r1cs_witness(address, path);
      if (pb.is_satisfied())
      {
        cout << "SATISFIED 1" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 1 (as expected)" << endl;
      }

      g.generate_r1cs_witness();
      if (pb.is_satisfied())
      {
        cout << "SATISFIED 2 (as expected)" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 2" << endl;
      }

      address_bits_va.fill_with_bits(pb, address_bits);
      prev_root_digest.generate_r1cs_witness(mt.root());
      leaf_digest.generate_r1cs_witness(leaf_bv);
      next_root_digest.generate_r1cs_witness(next_root_bv);
      path_var.generate_r1cs_witness(address, path);
      if (pb.is_satisfied())
      {
        cout << "SATISFIED 3 (as expected)" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 3" << endl;
      }

      libff::bit_vector another_leaf_bv = random_bits();
      leaf_digest.generate_r1cs_witness(another_leaf_bv);

      if (pb.is_satisfied())
      {
        cout << "SATISFIED 4" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 4 (as expected)" << endl;
      }

      address_bits_va.fill_with_bits(pb, address_bits);
      prev_root_digest.generate_r1cs_witness(mt.root());
      leaf_digest.generate_r1cs_witness(leaf_bv);
      libff::bit_vector another_root_bv = random_bits();
      next_root_digest.generate_r1cs_witness(another_root_bv);
      path_var.generate_r1cs_witness(address, path);
      if (pb.is_satisfied())
      {
        cout << "SATISFIED 5" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 5 (as expected)" << endl;
      }

      address_bits_va.fill_with_bits(pb, address_bits);
      prev_root_digest.generate_r1cs_witness(mt.root());
      leaf_digest.generate_r1cs_witness(leaf_bv);
      next_root_digest.generate_r1cs_witness(next_root_bv);
      path_var.generate_r1cs_witness(address, path);
      if (pb.is_satisfied())
      {
        cout << "SATISFIED 6 (as expected)" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 6" << endl;
      }

      mt.add(leaf_bv);
    }
    cout << endl;
  }
}
