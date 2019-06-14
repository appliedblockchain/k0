#ifndef ZKTRADE_UTIL_H
#define ZKTRADE_UTIL_H

#include <libff/common/utils.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>

using namespace libff;
using namespace libsnark;
using namespace std;

namespace zktrade {
    bit_vector byte_to_bits(unsigned char b);

    unsigned char bits_to_byte(bit_vector bv);

    bit_vector concat(bit_vector &a, bit_vector &b);

    bit_vector concat(bit_vector &a, bit_vector &b, bit_vector &c);

    bit_vector bytes_to_bits(vector<unsigned char> &bytes);

    vector<unsigned char> bits_to_bytes(bit_vector bits);

    string bytes_to_hex(vector<unsigned char> bytes);

    string bytes_to_hex(const unsigned char *bytes, const size_t len);

    void fill_with_bytes_of_hex_string(unsigned char *bytes,
                                       const std::string& hex);

    bit_vector random_bits(size_t len);

    void fill_with_random_bytes(unsigned char *bytes, size_t len);

    void fill_with_bits(unsigned char *bytes, bit_vector &bits);

    uint64_t random_uint64();

    bit_vector truncate(bit_vector input, size_t len);

    string bits_to_string(bit_vector input);

    string bits_to_hex(bit_vector input);

    bit_vector zero_bits(size_t len);

    string uint64_to_string(uint64_t val);

    template<typename FieldT>
    vector<FieldT> field_elements_from_bits(bit_vector bitv) {
        protoboard<FieldT> pb;
        pb_variable_array<FieldT> packed;
        packed.allocate(pb, 2, "packed");
        pb_variable_array<FieldT> bits;
        bits.allocate(pb, 256, "bits");
        multipacking_gadget<FieldT> packer(pb, bits, packed, 128, "packer");
        packer.generate_r1cs_constraints(false);
        pb.set_input_sizes(2);
        bits.fill_with_bits(pb, bitv);
        packer.generate_r1cs_witness_from_bits();
        return pb.primary_input;
    }

    template<typename FieldT>
    bit_vector
    bits_from_field_elements(vector<FieldT> field_elements) {
        protoboard<FieldT> pb;
        pb_variable_array<FieldT> packed;
        packed.allocate(pb, 2, "packed");
        pb_variable_array<FieldT> bits;
        bits.allocate(pb, 256, "bits");
        multipacking_gadget<FieldT> packer(pb, bits, packed, 128, "packer");
        packer.generate_r1cs_constraints(false);
        pb.set_input_sizes(256);
        packed.fill_with_field_elements(field_elements);
        packer.generate_r1cs_witness_from_packed();
        return pb.primary_input();
    }

    template<typename FieldT>
    FieldT bits_to_field_element(bit_vector &input)
    {
        protoboard<FieldT> pb;
        pb_variable<FieldT> packed;
        packed.allocate(pb, "packed");
        pb_variable_array<FieldT> bits;
        bits.allocate(pb, input.size(), "bits");
        packing_gadget<FieldT> packer(pb, bits, packed, "packer");
        packer.generate_r1cs_constraints(false);
        bits.fill_with_bits(pb, input);
        packer.generate_r1cs_witness_from_bits();
        return pb.val(packed);
    }

    template<typename FieldT>
    string field_element_to_string(FieldT el) {
        stringstream stream;
        stream << el;
        return stream.str();
    }

    template<typename FieldT>
    bit_vector field_element_to_64_bits(FieldT v) {
        protoboard<FieldT> pb;
        pb_variable<FieldT> v_number;
        v_number.allocate(pb, "v_number");
        pb_variable_array<FieldT> v_bits;
        v_bits.allocate(pb, 64, "v_bits");
        packing_gadget<FieldT> packer(pb, v_bits, v_number, "packing");

        pb.set_input_sizes(1);
        packer.generate_r1cs_constraints(false);
        pb.val(v_number) = v;
        packer.generate_r1cs_witness_from_packed();
        return v_bits.get_bits(pb);
    }

    // Returns the same result as the libsnark gadgetlib1 packer
    bit_vector uint64_to_bits(uint64_t input);

    // Returns the same result as the libsnark gadgetlib1 packer
    uint64_t bits_to_uint64(bit_vector input);


    template<typename FieldT>
    bit_vector int_to_bits(uint input, size_t bitlength)
    {
        protoboard<FieldT> pb;
        pb_variable<FieldT> packed;
        packed.allocate(pb, "packed");
        pb_variable_array<FieldT> bits;
        bits.allocate(pb, bitlength, "bits");
        packing_gadget<FieldT> packer(pb, bits, packed, "packer");
        packer.generate_r1cs_constraints(false);
        pb.val(packed) = input;
        packer.generate_r1cs_witness_from_packed();
        return bits.get_bits(pb);
    }

    // "0xdf1a27Fc2b2EA68525E3dcc5780CbcbE73e6778A" ->
    // "1273688244456584839735093243794471170958885812106"
    string hex_to_dec_string(string hex);

    void printnode(vector<bool> bv);

}

#endif //ZKTRADE_UTIL_H
