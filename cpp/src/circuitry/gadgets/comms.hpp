#ifndef ZKTRADE_GADGETS_COMMS_HPP
#define ZKTRADE_GADGETS_COMMS_HPP

using namespace libsnark;
using namespace std;
using namespace zktrade;

namespace zktrade {
    template<typename FieldT>
    pb_variable_array<FieldT> repeat(pb_variable<FieldT> v, size_t len) {
        pb_variable_array<FieldT> arr;
        for (size_t i = 0; i < len; i++) {
            arr.emplace_back(v);
        }
        return arr;
    }

    template<typename FieldT>
    class outer_commitment_gadget : gadget<FieldT> {
    private:
        sha256_compression_gadget <FieldT> cm_hasher;
    public:
        outer_commitment_gadget(protoboard<FieldT> &pb,
                                pb_variable<FieldT> &ZERO,
                                pb_variable_array<FieldT> &k,
                                pb_variable_array<FieldT> &v,
                                digest_variable<FieldT> &result,
                                const string &annotation_prefix) :
                gadget<FieldT>(pb, FMT(annotation_prefix, " gadget")),
                cm_hasher(pb, {k, repeat(ZERO, 192), v}, result,
                          FMT(annotation_prefix, " cm_hasher")) {
        }

        void generate_r1cs_constraints() {
            cm_hasher.generate_r1cs_constraints();
        }

        void generate_r1cs_witness() {
            cm_hasher.generate_r1cs_witness();
        }
    };
}
#endif //ZKTRADE_GADGETS_COMMS_HPP