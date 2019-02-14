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

    bit_vector random_bits(size_t len);

    bit_vector truncate(bit_vector input, size_t len);

    string bits_to_string(bit_vector input);

    string bits_to_hex(bit_vector input);

    bit_vector zero_bits(size_t len);

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
        return pb.primary_input();
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
    string field_element_to_string(FieldT el) {
        stringstream stream;
        stream << el;
        return stream.str();
    }
}

#endif //ZKTRADE_UTIL_H