#include <iostream>
#include <libff/common/utils.hpp>
#include <util.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "sha256.hpp"
#include "libsnark_hash.hpp"

using namespace std;
//
//void k0::sha256(unsigned char* input, unsigned char* output)
//{
//    CSHA256 hasher;
//    hasher.Write(input, 64);
//    hasher.Finalize(output);
//}

libff::bit_vector k0::sha256(libff::bit_vector &input) {

    typedef libff::Fr<libsnark::default_r1cs_ppzksnark_pp> FieldT;
    return k0::libsnark_hash<FieldT>(input);
//    if (input.size() != 512) {
//        throw invalid_argument("Input bit vector is not of length 512");
//    }
//    auto input_bytes_vec = bits_to_bytes(input);
//    unsigned char input_bytes_arr[64];
//    for (int i = 0; i < 64; i++) {
//        input_bytes_arr[i] = input_bytes_vec[i];
//    }
//    unsigned char output_bytes_arr[32];
//    k0::sha256(input_bytes_arr, output_bytes_arr);
//    vector<unsigned char> output_bytes_vec(32);
//    for (int i = 0; i < 32; i++) {
//        output_bytes_vec[i] = output_bytes_arr[i];
//    }
//    return bytes_to_bits(output_bytes_vec);
}
