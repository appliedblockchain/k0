#include <gtest/gtest.h>
#include "definitions.hpp"
#include "circuitry/TransferCircuit.hpp"

using namespace zktrade;

TEST(TransferCircuit, Test) {
    auto circuit = make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(2);
}