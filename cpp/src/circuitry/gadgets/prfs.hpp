#ifndef ZKTRADE_GADGETS_prfS_H
#define ZKTRADE_GADGETS_prfS_H

/*
 Copied from Zcash source code (MIT license)
 Modified by Applied Blockchain
 */

#include <libsnark/gadgetlib1/gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>

using namespace std;
using namespace libsnark;

namespace zktrade {
    template<typename FieldT>
    class prf_gadget : gadget<FieldT> {
    private:
        shared_ptr<block_variable<FieldT>> block;
        shared_ptr<sha256_compression_function_gadget < FieldT>> hasher;
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
            block.reset(new block_variable<FieldT>(
                    pb, {x, discriminants, z},
                    FMT(annotation_prefix, " block")));
            hasher.reset(new sha256_compression_function_gadget<FieldT>(
                    pb, IV, block->bits, *result,
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

    template<typename FieldT>
    class prf_addr_gadget : public prf_gadget<FieldT> {
    public:
        prf_addr_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> &a_sk,
                std::shared_ptr<digest_variable<FieldT>> result,
                const std::string &annotation_prefix
        ) : prf_gadget<FieldT>(pb, ZERO, a_sk, 0, 0, gen_zeros(ZERO, 254),
                               result, annotation_prefix) {}
    };

    template<typename FieldT>
    class prf_sn_gadget : public prf_gadget<FieldT> {
    public:
        prf_sn_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> &a_sk,
                pb_variable_array<FieldT> &rho,
                std::shared_ptr<digest_variable<FieldT>> result,
                const std::string &annotation_prefix
        ) : prf_gadget<FieldT>(pb, ZERO, a_sk, 0, 1,
                               pb_variable_array<FieldT>(rho.begin(),
                                                         rho.begin() + 254),
                               result, annotation_prefix) {}
    };
}
#endif //ZKTRADE_GADGETS_prfS_H
