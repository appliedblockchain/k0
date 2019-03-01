// Copied from libsnark
// Modified by Applied Blockchain

#ifndef ZKTRADE_DUMMYHASH_GADGET_HPP
#define ZKTRADE_DUMMYHASH_GADGET_HPP

#include <libsnark/common/data_structures/merkle_tree.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_components.hpp>

using namespace libsnark;

namespace zktrade {

template<typename FieldT>
class dummyhash_compression_function_gadget : public gadget<FieldT> {
public:
    std::vector<pb_linear_combination_array<FieldT> > round_a;
    std::vector<pb_linear_combination_array<FieldT> > round_b;
    std::vector<pb_linear_combination_array<FieldT> > round_c;
    std::vector<pb_linear_combination_array<FieldT> > round_d;
    std::vector<pb_linear_combination_array<FieldT> > round_e;
    std::vector<pb_linear_combination_array<FieldT> > round_f;
    std::vector<pb_linear_combination_array<FieldT> > round_g;
    std::vector<pb_linear_combination_array<FieldT> > round_h;

    pb_variable_array<FieldT> packed_W;
    std::shared_ptr<sha256_message_schedule_gadget<FieldT> > message_schedule;
    std::vector<sha256_round_function_gadget<FieldT> > round_functions;

    pb_variable_array<FieldT> unreduced_output;
    pb_variable_array<FieldT> reduced_output;
    std::vector<lastbits_gadget<FieldT> > reduce_output;
public:
    pb_linear_combination_array<FieldT> prev_output;
    pb_variable_array<FieldT> new_block;
    digest_variable<FieldT> output;

    dummyhash_compression_function_gadget(protoboard<FieldT> &pb,
                                       const pb_linear_combination_array<FieldT> &prev_output,
                                       const pb_variable_array<FieldT> &new_block,
                                       const digest_variable<FieldT> &output,
                                       const std::string &annotation_prefix);
    void generate_r1cs_constraints();
    void generate_r1cs_witness();
};


    template<typename FieldT>
    class dummyhash_two_to_one_hash_gadget : public gadget<FieldT> {
    public:
        typedef libff::bit_vector hash_value_type;
        typedef merkle_authentication_path merkle_authentication_path_type;

        std::shared_ptr<dummyhash_compression_function_gadget<FieldT> > f;

        dummyhash_two_to_one_hash_gadget(protoboard<FieldT> &pb,
                                      const digest_variable<FieldT> &left,
                                      const digest_variable<FieldT> &right,
                                      const digest_variable<FieldT> &output,
                                      const std::string &annotation_prefix);
        dummyhash_two_to_one_hash_gadget(protoboard<FieldT> &pb,
                                      const size_t block_length,
                                      const block_variable<FieldT> &input_block,
                                      const digest_variable<FieldT> &output,
                                      const std::string &annotation_prefix);

        void generate_r1cs_constraints(const bool ensure_output_bitness=true); // TODO: ignored for now
        void generate_r1cs_witness();

        static size_t get_block_len();
        static size_t get_digest_len();
        static libff::bit_vector get_hash(const libff::bit_vector &input);

        static size_t expected_constraints(const bool ensure_output_bitness=true); // TODO: ignored for now
    };

    template<typename FieldT>
    class dummyhash_compression_gadget : public gadget<FieldT> {
    private:
        std::unique_ptr<dummyhash_compression_function_gadget<FieldT> > f;
    public:
        dummyhash_compression_gadget(
                protoboard<FieldT> &pb,
                std::vector<pb_variable_array<FieldT>> inputs,
                digest_variable<FieldT> &result,
                const std::string &annotation_prefix
        );
        void generate_r1cs_constraints();
        void generate_r1cs_witness();
        static libff::bit_vector get_hash(const libff::bit_vector &input);
    };


} // zktrade

#include "dummyhash_gadget.tcc"

#endif // DUMMYHASH_GADGET_HPP_
