#ifndef K0_SUMEQUALITYGADGET_HPP
#define K0_SUMEQUALITYGADGET_HPP

#include <libsnark/gadgetlib1/gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_authentication_path_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_update_gadget.hpp>

using namespace libsnark;

template<typename FieldT, typename HashT>
class MTLeafAddition: public gadget<FieldT> {
   public:
        SumEqualityGadget(
                protoboard<FieldT> &pb,
                pb_variable_array<FieldT> &in_0,
                        pb_variable_array<FieldT> &in_1,
    pb_variable_array<FieldT> &out_0,
            pb_variable_array<FieldT> &out_0
    );
                       pb_variable<FieldT> &ZERO,
                       pb_variable_array<FieldT> &address_bits_va,
                       digest_variable<FieldT> &prev_root_digest,
                       digest_variable<FieldT> &leaf_digest,
                       digest_variable<FieldT> &next_root_digest,
                       merkle_authentication_path_variable<FieldT, HashT> &path_var);
        void generate_r1cs_constraints();
        void generate_r1cs_witness();
    };
};
private:
    pb_variable<FieldT> ZERO;

#endif //K0_SUMEQUALITYGADGET_HPP
