#include <libsnark/gadgetlib1/gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>

using namespace libsnark;

template<typename FieldT, typename HashT>
class MTLeafAddition: public gadget<FieldT> {
private:
  pb_variable<FieldT> ZERO;
  pb_variable_array<FieldT> address_bits_va;
  digest_variable<FieldT> prev_leaf_digest;
  digest_variable<FieldT> prev_root_digest;
  digest_variable<FieldT> next_leaf_digest;
  digest_variable<FieldT> next_root_digest;
  merkle_authentication_path_variable<FieldT, HashT> prev_path_var;
  merkle_authentication_path_variable<FieldT, HashT> next_path_var;
  merkle_tree_check_update_gadget<FieldT, HashT> mtcug;
public:
  MTLeafAddition(protoboard<FieldT> &pb,
                 size_t tree_height,
                 pb_variable<FieldT> &ZERO,
                 pb_variable_array<FieldT> &address_bits_va,
                 digest_variable<FieldT> &prev_root_digest,
                 digest_variable<FieldT> &leaf_digest,
                 digest_variable<FieldT> &next_root_digest,
                 merkle_authentication_path_variable<FieldT, HashT> &path_var);
  void generate_r1cs_constraints();
  void generate_r1cs_witness();
};

#include "MTLeafAddition.tcc"
