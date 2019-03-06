#include <gtest/gtest.h>
#include "definitions.hpp"
#include "circuitry/TransferCircuit.hpp"
#include "MerkleTree.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"

using namespace zktrade;

TEST(TransferCircuit, Test) {
    size_t tree_height = 3;
    auto circuit = make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(tree_height);
    MerkleTree<MerkleTreeHashT> mt(tree_height);

    print(circuit);
    input_note in[2];
    output_note out[2];

    for (size_t i = 0; i < 2; i++) {
        in[i].address = i;
        in[i].a_sk = random_bits(256);
        in[i].rho = random_bits(256);
        in[i].r = random_bits(384);
        in[i].v = 4;
        auto a_pk = prf_addr<CommitmentHashT>(in[i].a_sk);
        auto k = comm_r<CommitmentHashT>(a_pk, in[i].rho, in[i].r);
        auto cm = comm_s<CommitmentHashT>(k, uint64_to_bits(in[i].v));
        mt.add(cm);
    }

    for (size_t i = 0; i < 2; i++) {
        in[i].path = mt.path(i);
    }

     for (size_t i = 0; i < 2; i++) {
        out[i].a_pk = random_bits(256);
        out[i].rho = random_bits(256);
        out[i].r = random_bits(384);
        out[i].v = 4;
    }

     auto mt_root = mt.root();
    populate(circuit, tree_height, mt_root, in[0], in[1], out[0], out[1]);
    print(circuit);

}