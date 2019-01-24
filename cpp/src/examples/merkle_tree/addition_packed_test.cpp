#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "MTLeafAdditionPacked.hpp"
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

  for (size_t tree_height = 2; tree_height < 4; tree_height++)
  {
    cout << "TREE HEIGHT " << tree_height << endl;
    protoboard<FieldT> pb;

    pb_variable<FieldT> address_v;
    pb_variable_array<FieldT> prev_root_va;
    pb_variable_array<FieldT> leaf_va;
    pb_variable_array<FieldT> next_root_va;

    // cannot (?) put this into mtleafadditionpacked because must be initialised
    // (with a size) when the merkle update check gadget is initialised
    pb_variable_array<FieldT> address_bits_va;

    address_v.allocate(pb, "address");
    prev_root_va.allocate(pb, 2, "prev_root");
    leaf_va.allocate(pb, 2, "leaf");
    next_root_va.allocate(pb, 2, "next_root");
    address_bits_va.allocate(pb, tree_height, "address_bits");

    packing_gadget<FieldT> address_unpacker(pb, address_bits_va, address_v, "address_unpacker");

    merkle_authentication_path_variable<FieldT, HashT> path_var(pb, tree_height, "merkle_authentication_path");

    MTLeafAdditionPacked<FieldT, HashT> g(pb,
                                          tree_height,
                                          address_bits_va,
                                          prev_root_va,
                                          leaf_va,
                                          next_root_va,
                                          path_var);
    pb.set_input_sizes(7);

    address_unpacker.generate_r1cs_constraints(true);
    g.generate_r1cs_constraints();

    MerkleTree mt(tree_height);

    for (uint address = 0; address < exp2(tree_height); address++)
    {
      pb.val(address_v) = FieldT(address);
      address_unpacker.generate_r1cs_witness_from_packed();

      auto prev_root_bv = mt.root();
      auto prev_root_field_elems = field_elements_from_bits<FieldT>(prev_root_bv);
      prev_root_va.fill_with_field_elements(pb, prev_root_field_elems);

      libff::bit_vector leaf_bv = random_bits();
      auto leaf_elems = field_elements_from_bits<FieldT>(leaf_bv);
      leaf_va.fill_with_field_elements(pb, leaf_elems);

      auto simulation_result = mt.simulate_add(leaf_bv);
      libff::bit_vector next_root_bv = get<1>(simulation_result);
      vector<libff::bit_vector> path = get<2>(simulation_result);

      auto next_root_elems = field_elements_from_bits<FieldT>(next_root_bv);
      next_root_va.fill_with_field_elements(pb, next_root_elems);

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

      pb.val(address_v) = FieldT(address);
      address_unpacker.generate_r1cs_witness_from_packed();
      prev_root_va.fill_with_field_elements(pb, prev_root_field_elems);
      leaf_va.fill_with_field_elements(pb, leaf_elems);
      next_root_va.fill_with_field_elements(pb, next_root_elems);
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
      auto another_leaf_elems = field_elements_from_bits<FieldT>(another_leaf_bv);
      leaf_va.fill_with_field_elements(pb, another_leaf_elems);
      if (pb.is_satisfied())
      {
        cout << "SATISFIED 4" << endl;
      }
      else
      {
        cout << "NOT SATISFIED 4 (as expected)" << endl;
      }

      mt.add(leaf_bv);
    }
  }
}
