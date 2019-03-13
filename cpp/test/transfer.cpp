#include <gtest/gtest.h>
#include "circuitry/TransferCircuit.hpp"
#include "circuitry/gadgets/dummyhash_gadget.hpp"
#include "MerkleTree.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"

using namespace zktrade;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

typedef sha256_compression_gadget<FieldT> CommitmentHashT;
typedef sha256_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

TEST(TransferCircuit, CannotCreateMoney) {
    size_t tree_height = 3;
    MerkleTree<MerkleTreeHashT> mt(tree_height);

    input_note in[2];
    output_note out[2];

    uint64_t total = bits_to_uint64(random_bits(64));
    in[0].v = (total / 100) * (rand() % 100);
    in[1].v = total - in[0].v;

    for (size_t i = 0; i < 2; i++) {
        in[i].address = i;
        in[i].a_sk = random_bits(256);
        in[i].rho = random_bits(256);
        in[i].r = random_bits(384);
        auto a_pk = prf_addr<CommitmentHashT>(in[i].a_sk);
        auto k = comm_r<CommitmentHashT>(a_pk, in[i].rho, in[i].r);
        auto cm = comm_s<CommitmentHashT>(k, uint64_to_bits(in[i].v));
        mt.add(cm);
    }

    cout << "in 0 " << in[0].v << endl;
    cout << "in 1 " << in[1].v << endl;

    for (size_t i = 0; i < 2; i++) {
        in[i].path = mt.path(i);
    }

    for (size_t i = 0; i < 2; i++) {
        out[i].a_pk = random_bits(256);
        out[i].rho = random_bits(256);
        out[i].r = random_bits(384);
    }

    out[0].v = (total / 100) * (rand() % 100);
    auto out_1_v  = total - out[0].v;

    // try to create 1 additional value unit
    out[1].v = out_1_v + 1;

    auto mt_root = mt.root();
    auto circuit = make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(
            tree_height);
    FieldT callee("0");
    populate(circuit, tree_height, mt_root, in[0], in[1], out[0], out[1], callee);
    ASSERT_FALSE(circuit.pb->is_satisfied());
    generate_witness(circuit);
    ASSERT_FALSE(circuit.pb->is_satisfied());

    // set correct output value
    out[1].v = out_1_v;
    circuit = make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(
            tree_height);
    populate(circuit, tree_height, mt_root, in[0], in[1], out[0], out[1], callee);
    generate_witness(circuit);
    ASSERT_TRUE(circuit.pb->is_satisfied());
}