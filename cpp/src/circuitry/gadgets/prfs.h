#ifndef ZKTRADE_GADGETS_PRFS_H
#define ZKTRADE_GADGETS_PRFS_H

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
    class PRF_gadget : gadget<FieldT> {
    private:
        shared_ptr<block_variable<FieldT>> block;
        shared_ptr<sha256_compression_function_gadget < FieldT>> hasher;
        shared_ptr<digest_variable<FieldT>> result;
    public:
        PRF_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> x,
                bool a,
                bool b,
                pb_variable_array<FieldT> y,
                shared_ptr<digest_variable<FieldT>> result
        ) : gadget<FieldT>(pb), result(result) {
            pb_linear_combination_array<FieldT> IV = SHA256_default_IV(pb);
            pb_variable_array<FieldT> discriminants;
            discriminants.emplace_back(a ? ONE : ZERO);
            discriminants.emplace_back(b ? ONE : ZERO);
            block.reset(new block_variable<FieldT>(pb, {x, discriminants, y},
                                                   "PRF_block"));
            hasher.reset(new sha256_compression_function_gadget<FieldT>(pb,
                                                                        IV,
                                                                        block->bits,
                                                                        *result,
                                                                        "PRF_hasher"));
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
    class PRF_addr_gadget : public PRF_gadget<FieldT> {
    public:
        PRF_addr_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> &a_sk,
                std::shared_ptr<digest_variable<FieldT>> result
        ) : PRF_gadget<FieldT>(pb, ZERO, a_sk, 0, 0, gen_zeros(ZERO, 254),
                               result) {}
    };

    template<typename FieldT>
    class PRF_sn_gadget : public PRF_gadget<FieldT> {
    public:
        PRF_sn_gadget(
                protoboard<FieldT> &pb,
                pb_variable<FieldT> &ZERO,
                pb_variable_array<FieldT> &a_sk,
                pb_variable_array<FieldT> &rho,
                std::shared_ptr<digest_variable<FieldT>> result
        ) : PRF_gadget<FieldT>(pb, ZERO, a_sk, 0, 1, rho, result) {}
    };
}
#endif //ZKTRADE_GADGETS_PRFS_H
