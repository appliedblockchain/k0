#ifndef K0_GADGETS_COMMS_HPP
#define K0_GADGETS_COMMS_HPP

#include "circuitry/gadgets/sha256_compression.h"

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

    template<typename FieldT, typename HashT>
    class comm_r_gadget : gadget<FieldT> {
    private:
        digest_variable<FieldT> a_pk_rho_hash;
        HashT a_pk_rho_hasher;
        pb_variable_array<FieldT> a_pk_rho_hash_truncated;
        HashT k_hasher;
    public:
        comm_r_gadget(protoboard<FieldT> &pb,
                      pb_variable_array<FieldT> &a_pk,
                      pb_variable_array<FieldT> &rho,
                      pb_variable_array<FieldT> &r,
                      digest_variable<FieldT> &result,
                      const string &annotation_prefix)
                : gadget<FieldT>(pb, annotation_prefix),
                  a_pk_rho_hash(pb, 256,
                                FMT(annotation_prefix, " a_pk_rho_hash")),
                  a_pk_rho_hasher(pb, {a_pk, rho}, a_pk_rho_hash,
                                  FMT(annotation_prefix, " a_pk_rho_hasher")),
                  a_pk_rho_hash_truncated(a_pk_rho_hash.bits.begin(),
                                          a_pk_rho_hash.bits.begin() + 128),
                  k_hasher(pb, {r, a_pk_rho_hash_truncated}, result,
                           FMT(annotation_prefix, " k_hasher")) {
        }

        void generate_r1cs_constraints() {
            a_pk_rho_hasher.generate_r1cs_constraints();
            k_hasher.generate_r1cs_constraints();
        }

        void generate_r1cs_witness() {
            a_pk_rho_hasher.generate_r1cs_witness();
            k_hasher.generate_r1cs_witness();
        }
    };


    template<typename FieldT, typename HashT>
    class comm_s_gadget : gadget<FieldT> {
    private:
        HashT cm_hasher;
    public:
        comm_s_gadget(protoboard<FieldT> &pb,
                      pb_variable<FieldT> &ZERO,
                      pb_variable_array<FieldT> &k,
                      pb_variable_array<FieldT> &v,
                      digest_variable<FieldT> &result,
                      const string &annotation_prefix) :
                gadget<FieldT>(pb, FMT(annotation_prefix, " x gadget")),
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

    template<typename FieldT, typename HashT>
    class cm_full_gadget : gadget<FieldT> {
    private:
        digest_variable<FieldT> k;
        comm_r_gadget<FieldT, HashT> kg;
        comm_s_gadget<FieldT, HashT> outer_gadget;
    public:
        cm_full_gadget(protoboard<FieldT> &pb,
                       pb_variable<FieldT> &ZERO,
                       pb_variable_array<FieldT> &a_pk,
                       pb_variable_array<FieldT> &rho,
                       pb_variable_array<FieldT> &r,
                       pb_variable_array<FieldT> &v,
                       digest_variable<FieldT> &result,
                       const string &annotation_prefix)
                : gadget<FieldT>(pb, annotation_prefix),
                  k(pb, 256, FMT(annotation_prefix, " k")),
                  kg(pb, a_pk, rho, r, k, FMT(annotation_prefix, " comm_r_gadget")),
                  outer_gadget(pb, ZERO, k.bits, v, result,
                               FMT(annotation_prefix, " outer_gadget")) {
        }

        void generate_r1cs_constraints() {
            k.generate_r1cs_constraints();
            kg.generate_r1cs_constraints();
            outer_gadget.generate_r1cs_constraints();
        }

        void generate_r1cs_witness() {
            kg.generate_r1cs_witness();
            outer_gadget.generate_r1cs_witness();
        }

        libff::bit_vector k_bits() {
            return k.get_digest();
        }
    };

}
#endif //K0_GADGETS_COMMS_HPP
