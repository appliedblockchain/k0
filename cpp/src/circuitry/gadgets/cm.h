#ifndef ZKTRADE_GADGETS_CM_H
#define ZKTRADE_GADGETS_CM_H

#include <libsnark/gadgetlib1/gadget.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_components.hpp>
#include "sha256_compression.h"
#include "comms.hpp"

using namespace libsnark;

namespace zktrade {
    template<typename FieldT>
    class cm_gadget : gadget<FieldT> {
    private:
        digest_variable<FieldT> a_pk_rho_hash;
        sha256_compression_gadget<FieldT> a_pk_rho_hasher;
        pb_variable_array<FieldT> a_pk_rho_hash_truncated;
        digest_variable<FieldT> k;
        sha256_compression_gadget<FieldT> k_hasher;
        outer_commitment_gadget<FieldT> outer_gadget;
    public:
        cm_gadget(protoboard<FieldT> &pb,
                  pb_variable<FieldT> &ZERO,
                  pb_variable_array<FieldT> &a_pk,
                  pb_variable_array<FieldT> &rho,
                  pb_variable_array<FieldT> &r,
                  pb_variable_array<FieldT> &v,
                  digest_variable<FieldT> &result) :
                gadget<FieldT>(pb, "cm"),
                a_pk_rho_hash(pb, 256, "a_pk_rho_hash"),
                a_pk_rho_hasher(pb, a_pk, rho, a_pk_rho_hash,
                                "a_pk_rho_hasher"),
                a_pk_rho_hash_truncated(a_pk_rho_hash.bits.begin(),
                                        a_pk_rho_hash.bits.begin() + 128),
                k(pb, 256, "k"),
                k_hasher(pb, r, a_pk_rho_hash_truncated, k, "k_hasher"),
                outer_gadget(pb, ZERO, k.bits, v, result, "outer_gadget") {
        }

        void generate_r1cs_constraints() {
            a_pk_rho_hasher.generate_r1cs_constraints();
            k_hasher.generate_r1cs_constraints();
            outer_gadget.generate_r1cs_constraints();
        }

        void generate_r1cs_witness() {
            a_pk_rho_hasher.generate_r1cs_witness();
            k_hasher.generate_r1cs_witness();
            outer_gadget.generate_r1cs_witness();
        }
    };
}

#endif //ZKTRADE_GADGETS_CM_H
