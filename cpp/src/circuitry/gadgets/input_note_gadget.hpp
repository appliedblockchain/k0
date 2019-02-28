#ifndef ZKTRADE_INPUT_NOTE_GADGET_HPP
#define ZKTRADE_INPUT_NOTE_GADGET_HPP

namespace zktrade {

    template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
    class input_note_gadget : public gadget<FieldT> {
    private:
    public:
        input_note_gadget(
                protoboard<FieldT> &pb
        );

        void generate_r1cs_constraints();

        void generate_r1cs_witness();
    };

}

#include "input_note_gadget.tcc"

#endif //ZKTRADE_INPUT_NOTE_GADGET_HPP
