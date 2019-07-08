#include "sha256_compression.hpp"
#include "libsnark_hash.hpp"
#include <iostream>
#include <libff/common/utils.hpp>
#include <util.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>

using namespace std;
//
//void k0::sha256_compress(unsigned char* input, unsigned char* output)
//{
//    CSHA256 hasher;
//    hasher.Write(input, 64);
//    hasher.FinalizeNoPadding(output);
//}

libff::bit_vector k0::sha256_compress(libff::bit_vector input)
{
    if (input.size() != 512) {
        throw invalid_argument("Input bit vector is not of length 512");
    }

    typedef libff:: Fr<libsnark::default_r1cs_ppzksnark_pp> FieldT;
    return k0::libsnark_hash<FieldT>(input);
//
//    auto input_bytes_vec = bits_to_bytes(input);
//    unsigned char input_bytes_arr[64];
//    for (int i = 0; i < 64; i++) {
//        input_bytes_arr[i] = input_bytes_vec[i];
//    }
//    unsigned char output_bytes_arr[32];
//    k0::sha256_compress(input_bytes_arr, output_bytes_arr);
//    vector<unsigned char> output_bytes_vec(32);
//    for (int i = 0; i < 32; i++) {
//        output_bytes_vec[i] = output_bytes_arr[i];
//    }
//    return bytes_to_bits(output_bytes_vec);
}
