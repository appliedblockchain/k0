#include <gtest/gtest.h>
#include "definitions.hpp"
#include "serialization.hpp"
#include "MerkleTree.hpp"

TEST(MerkleTree, Test
) {
// We'll successively add these leaves
    vector<libff::bit_vector> leaves_to_add =
            hex2bitsVec(
                    {"0x91d9bafe6e8313848a4eb5f7961fd5682d83bde4ae03116ce96bd0c2157c251f",
                     "0xd0af92e34e8761b0a800d87e93b77eabd59e3eb8b91ff3806b6abec0dcc53ad8",
                     "0x33ff8776fc5cb9d2e08aa1a1f921017d825c91982bd7c7ca86bad745b314b243",
                     "0x91139f2302b182200a2c94fecd52770d22e8f9af6c487cabe652be98bab6b385",
                     "0xcd886c9c2fee18ae9d60ae17d1f7e0d4fd42dc28c325504c396293f719cca22b",
                     "0xcb7c44d1f9762dd8fd533188a8a291a1372afe94dd33ba3e7d86a1568377a741",
                     "0x29d4c64cc125b91bb62e5b6a16866babd361a0c7066fac94b9f4c827a62d3328",
                     "0x561da2af4d8dfa3bee3c78e87b1f2c5239a8c0bb8a089338853edd7766aae0f0"
                    });


    MerkleTree<MerkleTreeHashT> mt(3);

    cout << "ROOT BEFORE " << bits2hex(mt.root()) << endl;
    for (uint i = 0; i < 8; i++) {
        mt.add(leaves_to_add[i]);
        cout << "Root after " << i << bits2hex(mt.root()) << endl;
    }
}