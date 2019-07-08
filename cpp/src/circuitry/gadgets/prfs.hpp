#ifndef K0_GADGETS_PRFS_HPP
#define K0_GADGETS_PRFS_HPP

/*
 Copied from Zcash source code (MIT license)
 Modified by Applied Blockchain
 */

#include <libsnark/gadgetlib1/gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>

using namespace std;
using namespace libsnark;

namespace zktrade {
    template<typename FieldT, typename HashT>
    class prf_gadget : gadget<FieldT> {
    private:
        shared_ptr<HashT> hasher;
        shared_ptr<digest_variable<FieldT>> result;
    public:
        prf_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> x,
                bool a,
                bool b,
                pb_variable_array<FieldT> z,
                shared_ptr<digest_variable<FieldT>> result,
                const std::string &annotation_prefix
        ) : gadget<FieldT>(pb, FMT(annotation_prefix, " gadget")),
            result(result) {
            if (x.size() != 256) {
                throw invalid_argument("x not of length 256");
            }
            if (z.size() != 254) {
                throw invalid_argument("z not of length 254");
            }
            pb_linear_combination_array<FieldT> IV = SHA256_default_IV(pb);
            pb_variable_array<FieldT> discriminants;
            discriminants.emplace_back(a ? ONE : ZERO);
            discriminants.emplace_back(b ? ONE : ZERO);
            hasher.reset(new HashT(
                    pb, {x, discriminants, z}, *result,
                    FMT(annotation_prefix, " hasher")));
        }

        void generate_r1cs_constraints() {
            hasher->generate_r1cs_constraints();
        }

        void generate_r1cs_witness() {
            hasher->generate_r1cs_witness();
        }
    };

    template<typename FieldT>
    pb_variable_array<FieldT> gen_zeros(pb_variable<FieldT> &ZERO, size_t n) {
        pb_variable_array<FieldT> ret;
        while (ret.size() < n) {
            ret.emplace_back(ZERO);
        }
        return ret;
    }

    template<typename FieldT, typename HashT>
    class prf_addr_gadget : public prf_gadget<FieldT, HashT> {
    public:
        prf_addr_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> &a_sk,
                std::shared_ptr<digest_variable<FieldT>> result,
                const std::string &annotation_prefix
        ) : prf_gadget<FieldT, HashT>(pb, ZERO, a_sk, 0, 0,
                                      gen_zeros(ZERO, 254),
                                      result, annotation_prefix) {}
    };

    template<typename FieldT, typename HashT>
    class prf_sn_gadget : public prf_gadget<FieldT, HashT> {
    public:
        prf_sn_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> &a_sk,
                pb_variable_array<FieldT> &rho,
                std::shared_ptr<digest_variable<FieldT>> result,
                const std::string &annotation_prefix
        ) : prf_gadget<FieldT, HashT>(pb, ZERO, a_sk, 0, 1,
                               pb_variable_array<FieldT>(rho.begin(),
                                                         rho.begin() + 254),
                               result, annotation_prefix) {}
    };
}
#endif //K0_GADGETS_PRFS_HPP
