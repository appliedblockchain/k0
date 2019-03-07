#ifndef ZKTRADE_INPUT_NOTE_GADGET_HPP
#define ZKTRADE_INPUT_NOTE_GADGET_HPP

#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include "circuitry/gadgets/comms.hpp"
#include "circuitry/gadgets/prfs.hpp"

namespace zktrade {

    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    class input_note_gadget : public gadget<FieldT> {
    private:
        prf_addr_gadget<FieldT, CommitmentHashT> addr_gadget;
        cm_full_gadget<FieldT, CommitmentHashT> commitment_gadget;
        merkle_tree_check_read_gadget<FieldT, MerkleTreeHashT> mt_path_gadget;
        prf_sn_gadget<FieldT, CommitmentHashT> sn_gadget;
    public:
        input_note_gadget(
                const size_t &tree_height,
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                digest_variable<FieldT> &rt_bits,
                pb_variable_array<FieldT> &v_bits,
                pb_variable_array<FieldT> &a_sk_bits,
                pb_variable_array<FieldT> &rho_bits,
                pb_variable_array<FieldT> &r_bits,
                pb_variable_array<FieldT> &address_bits,
                merkle_authentication_path_variable<FieldT, MerkleTreeHashT> &path,
                shared_ptr<digest_variable<FieldT>> a_pk_bits,
                digest_variable<FieldT> &cm_bits,
                shared_ptr<digest_variable<FieldT>> sn_bits,
                const string &annotation_prefix
        );

        void generate_r1cs_constraints();

        void generate_r1cs_witness();
    };

}

#include "input_note_gadget.tcc"

#endif //ZKTRADE_INPUT_NOTE_GADGET_HPP
