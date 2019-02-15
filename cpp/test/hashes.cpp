#include <fstream>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "hash/sha256.hpp"
#include "hash/sha256_compression.hpp"
#include "circuitry/gadgets/sha256_512_bits.hpp"
#include "util.h"

using namespace std;
using namespace libff;
using namespace libsnark;
using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

string bytes_to_hex(unsigned char bytes[], size_t len) {
    stringstream stream;
    stream << "0x";
    for (size_t i = 0; i < len; i++) {
        uint number = bytes[i];
        stream << hex << setfill('0') << setw(2) << number;
    }
    return stream.str();
}

void hex_to_bytes(unsigned char *result, string input, size_t len) {
    for (size_t i = 0; i < len; i++) {
        string byteString = input.substr(2 + i * 2, 2);
        result[i] = (unsigned char) strtol(byteString.c_str(), NULL, 16);
    }
};

string random_bytes_hex(size_t len) {
    stringstream stream;
    for (size_t i = 0; i < len; i++) {
        uint8_t val = rand();
        stream << hex << setfill('0') << setw(2) << (int) val;
    }
    return stream.str();
}

string get_python_hash(string hash_function_name, string input_hex) {
    string filename = random_bytes_hex(32);
    string path = "/tmp/" + filename;
    const char *base_dir = std::getenv("BASE_DIR");
    if (base_dir == nullptr) {
        base_dir = ".";
    }
    string command = "python3 " + (string) base_dir +
                     "/test/aux/" + hash_function_name + ".py " +
                     input_hex + " > " + path;
    int python_run_result = system(command.c_str());
    if (python_run_result != 0) {
        throw runtime_error("Could not run Python hash program");
    }
    std::ifstream infile(path);
    std::string result;
    getline(infile, result);
    int remove_result = remove(path.c_str());
    if (remove_result != 0) {
        string reason =
                (string) "Could not delete temporary file with Python " +
                (string) "hash program output";
        throw runtime_error(reason);
    }
    return result;
}
// This just checks that the Python implementation works
TEST(SHA256Compression, CorrectnessOfPythonImplementation) {
    struct example {
        string input_hex;
        string expected_hash_hex;
    };

    vector<example> examples = {
            {
                    "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                    "0xda5698be17b9b46962335799779fbeca8ce5d491c0d26243bafef9ea1837a9d8"
            },
            {
                    "0x11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
                    "0x56a8efdb498db598e48ab95d39110e9bc1d61b89810a21b33dc59d72f8f04c37"
            },
            {
                    "0xa7f1d92a82c8d8fe434d98558ce2b347171198542f112d0558f56bd68807999248336241f30d23e55f30d1c8ed610c4b0235398184b814a29cb45a672acae548",
                    "0x64dff498e17c28523afcfb29216a43785088f2f89b90f02ba4e1b6001f31ad3d"
            }
    };
    for (auto ex : examples) {
        string output_python_hex = get_python_hash("sha256_compress",
                                                   ex.input_hex);
        ASSERT_EQ(output_python_hex, ex.expected_hash_hex);
    }
}

TEST(SHA256Compression, EqualityOfImplementations) {
    for (int i = 0; i < 1; i++) {
        string input_hex = "0x" + random_bytes_hex(64);
        string output_python_hex = get_python_hash("sha256_compress",
                                                   input_hex);

        unsigned char input[64];
        hex_to_bytes(input, input_hex, 64);
        unsigned char output_bitcoin[32];
        sha256_compress(input, output_bitcoin);
        string output_bitcoin_hex = bytes_to_hex(output_bitcoin, 32);
        ASSERT_EQ(output_bitcoin_hex, output_python_hex);

        protoboard<FieldT> pb;
        pb_linear_combination_array<FieldT> IV = SHA256_default_IV(pb);
        pb_variable_array<FieldT> block;
        block.allocate(pb, 512, "block");
        digest_variable<FieldT> result(pb, 256, "result");
        sha256_compression_function_gadget<FieldT> hasher(pb, IV, block, result,
                                                          "hasher");
        hasher.generate_r1cs_constraints();
        ASSERT_FALSE(pb.is_satisfied());

        vector<unsigned char> input_v{std::begin(input), std::end(input)};
        bit_vector input_bits = bytes_to_bits(input_v);
        block.fill_with_bits(pb, input_bits);
        hasher.generate_r1cs_witness();
        ASSERT_TRUE(pb.is_satisfied());

        vector<unsigned char> result_bytes = bits_to_bytes(result.get_digest());
        string output_gadget_hex = bytes_to_hex(result_bytes);
        ASSERT_EQ(output_gadget_hex, output_python_hex);
    }
}

// This just checks that the Python implementation works
TEST(SHA256, CorrectnessOfPythonImplementation) {
    struct example {
        string input_hex;
        string expected_hash_hex;
    };

    vector<example> examples = {
            {
                    "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                    "0xf5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b"
            },
            {
                    "0x11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
                    "0x9aed5fce4bb60c40cb8a2983b43540adb4c8ac8aa1ef1f20de57526f9ed86e38"
            },
            {
                    "0xa7f1d92a82c8d8fe434d98558ce2b347171198542f112d0558f56bd68807999248336241f30d23e55f30d1c8ed610c4b0235398184b814a29cb45a672acae548",
                    "0xd3cf41ada01dfd7ed38d340f012302f5fb069d93f7e2c3e7b3852cf48fc727e7"
            }
    };
    for (auto ex : examples) {
        string output_python_hex = get_python_hash("sha256", ex.input_hex);
        ASSERT_EQ(output_python_hex, ex.expected_hash_hex);
    }
}

TEST(SHA256, EqualityOfImplementations) {
    for (int i = 0; i < 1; i++) {
        string input_hex = "0x" + random_bytes_hex(64);
        string output_python_hex = get_python_hash("sha256", input_hex);

        unsigned char input[64];
        hex_to_bytes(input, input_hex, 64);
        unsigned char output_bitcoin[32];
        sha256(input, output_bitcoin);
        string output_bitcoin_hex = bytes_to_hex(output_bitcoin, 32);
        ASSERT_EQ(output_bitcoin_hex, output_python_hex);

        protoboard<FieldT> pb;
        pb_variable<FieldT> ZERO;
        ZERO.allocate(pb, "ZERO");
        block_variable<FieldT> block(pb, 512, "block");
        digest_variable<FieldT> result(pb, 256, "result");
        sha256_512_bits_gadget<FieldT> hasher(pb, ZERO, block, result,
                                              "hasher");
        hasher.generate_r1cs_constraints();
        ASSERT_FALSE(pb.is_satisfied());
        vector<unsigned char> input_bytes_v{begin(input), end(input)};
        block.generate_r1cs_witness(bytes_to_bits(input_bytes_v));
        hasher.generate_r1cs_witness();
        ASSERT_TRUE(pb.is_satisfied());
        ASSERT_EQ(bits_to_hex(result.get_digest()), output_python_hex);
    }
}