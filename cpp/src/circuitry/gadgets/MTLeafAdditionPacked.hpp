#include "MTLeafAddition.hpp"

template<typename FieldT, typename HashT>
class MTLeafAdditionPacked : public gadget<FieldT> {
private:
  digest_variable<FieldT> prev_root_digest;
  multipacking_gadget<FieldT> prev_root_unpacker;
  digest_variable<FieldT> leaf_digest;
  multipacking_gadget<FieldT> leaf_unpacker;
  digest_variable<FieldT> next_root_digest;
  multipacking_gadget<FieldT> next_root_unpacker;
  MTLeafAddition<FieldT, HashT> mtla;
public:
  MTLeafAdditionPacked(protoboard<FieldT> &pb,
                       const size_t tree_height,
                       const pb_variable_array<FieldT> &address_bits_va,
                       const pb_variable_array<FieldT> &prev_root_va,
                       const pb_variable_array<FieldT> &leaf_va,
                       const pb_variable_array<FieldT> &next_root_va,
                       const merkle_authentication_path_variable<FieldT, HashT> &path_var);
  void generate_r1cs_constraints();
  void generate_r1cs_witness();
};

#include "MTLeafAdditionPacked.tcc"
