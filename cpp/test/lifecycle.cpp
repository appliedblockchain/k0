#include <gtest/gtest.h>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "circuitry/gadgets/dummyhash_gadget.hpp"
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/MTAdditionCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "circuitry/TransferCircuit.hpp"
#include "MerkleTree.hpp"
#include "scheme/comms.hpp"
#include "scheme/prfs.h"
#include "util.h"

using namespace zktrade;
using namespace libsnark;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;

typedef sha256_compression_gadget<FieldT> CommitmentHashT;
typedef sha256_two_to_one_hash_gadget<FieldT> MerkleTreeHashT;

TEST(Lifecycle, Full) {

    struct coin {
        size_t address;
        bit_vector rho;
        bit_vector r;
        bit_vector a_sk;
        uint64_t v;
    };

    vector<coin> coins = {
            {
                    0,
                    hex2bits(
                            "0x0101010101010101010101010101010101010101010101010101010101010101"),
                    hex2bits(
                            "0x020202020202020202020202020202020202020202020202020202020202020202020202020202020202020202020202"),
                    hex2bits(
                            "0x0303030303030303030303030303030303030303030303030303030303030303"),
                    0
            },
            {
                    1,
                    hex2bits(
                            "0x1111111111111111111111111111111111111111111111111111111111111111"),
                    hex2bits(
                            "0x121212121212121212121212121212121212121212121212121212121212121212121212121212121212121212121212"),
                    hex2bits(
                            "0x1313131313131313131313131313131313131313131313131313131313131313"),
                    100000000000
            },
            {
                    2,
                    hex2bits(
                            "0x2121212121212121212121212121212121212121212121212121212121212121"),
                    hex2bits(
                            "0x222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"),
                    hex2bits(
                            "0x2323232323232323232323232323232323232323232323232323232323232323"),
                    200000000000
            },
            {
                    3,
                    hex2bits(
                            "0x3131313131313131313131313131313131313131313131313131313131313131"),
                    hex2bits(
                            "0x323232323232323232323232323232323232323232323232323232323232323232323232323232323232323232323232"),
                    hex2bits(
                            "0x3333333333333333333333333333333333333333333333333333333333333333"),
                    300000000000
            }

    };

    size_t tree_height = 3;

    MerkleTree<MerkleTreeHashT> mt(tree_height);

    cout << "Initial Merkle tree root " << bits2hex(mt.root()) << endl;

    //size_t num_initial_coins = exp2(tree_height) / 2;
    size_t num_initial_coins = 2;

    for (size_t address = 0; address < num_initial_coins; address++) {

        cout << endl << "ADDING " << address << endl;

        // Sample private key
//        auto a_sk = random_bits(256);
        auto a_sk = coins[address].a_sk;
        // Derive public key/address
        auto a_pk = prf_addr_a_pk<CommitmentHashT>(a_sk);

        // DEPOSIT/"SHIELDING"
        // Sample rho
//        auto rho = random_bits(256);
        auto rho = coins[address].rho;
        // Sample r
//        auto r = random_bits(384);
        auto r = coins[address].r;
        // Generate k
        auto k = comm_r<CommitmentHashT>(a_pk, rho, r);
        cout << "k " << bits2hex(k) << endl;

        // Set v
        //      uint64_t v_uint = 5000000000000000000;
        uint64_t v_uint = coins[address].v;
        string v_str = to_string(v_uint);
        FieldT v = FieldT(v_str.c_str());
        bit_vector v_bits = uint64_to_bits(stoull(v_str));

        // Generate cm
        auto cm = comm_s<CommitmentHashT>(k, v_bits);

        cout << "cm " << bits2hex(cm) << endl;
        //coin c{address, rho, r, a_sk, v_uint};
        //coins.push_back(c);

        ASSERT_EQ(mt.num_elements(), address);

        bit_vector address_bits = int_to_bits<FieldT>(address, tree_height);
        // Add cm to the tree
        auto sim_result = mt.simulate_add(cm);


        // Generate deposit proof
        auto comm_circuit = make_commitment_circuit<FieldT, CommitmentHashT>();
        comm_circuit.k_bits->fill_with_bits(*comm_circuit.pb, k);
        comm_circuit.k_packer->generate_r1cs_witness_from_bits();
        comm_circuit.pb->val(*comm_circuit.v_packed) = v;
        comm_circuit.v_packer->generate_r1cs_witness_from_packed();
        //ASSERT_FALSE(comm_circuit.pb->is_satisfied());

        comm_circuit.commitment_gadget->generate_r1cs_witness();
        comm_circuit.cm_packer->generate_r1cs_witness_from_bits();
        ASSERT_TRUE(comm_circuit.pb->is_satisfied());
        ASSERT_EQ(comm_circuit.cm_bits->get_digest(), cm);

        auto cm_packed = comm_circuit.cm_packed->get_vals(*comm_circuit.pb);

        auto add_circuit = make_mt_addition_circuit<FieldT, MerkleTreeHashT>(
                tree_height);

        add_circuit.prev_root_bits->generate_r1cs_witness(mt.root());
        add_circuit.prev_root_packer->generate_r1cs_witness_from_bits();

        add_circuit.pb->val(*add_circuit.address_packed) = address;
        add_circuit.address_packer->generate_r1cs_witness_from_packed();

        add_circuit.prev_path_var->generate_r1cs_witness(address,
                                                         mt.path(address));

        add_circuit.next_leaf_packed->fill_with_field_elements(*add_circuit.pb,
                                                               cm_packed);
        add_circuit.next_leaf_packer->generate_r1cs_witness_from_packed();
        ASSERT_EQ(add_circuit.next_leaf_bits->get_digest(), cm);

        add_circuit.next_root_bits->generate_r1cs_witness(get<1>(sim_result));
        add_circuit.next_root_packer->generate_r1cs_witness_from_bits();

        //ASSERT_FALSE(add_circuit.pb->is_satisfied());
        add_circuit.mt_update_gadget->generate_r1cs_witness();

        ASSERT_TRUE(add_circuit.pb->is_satisfied());

        cout << "Root before " << bits2hex(mt.root()) << endl;
        //mt.print();
        cout << "Adding      " << bits2hex(cm) << endl;
        mt.add(cm);
        cout << "Root after  " << bits2hex(mt.root()) << endl;
        //mt.print();
    }


    // TRANSFER
    for (size_t input_0_address = 0;
         input_0_address < num_initial_coins; input_0_address += 2) {

        cout << "Transfer from address " << input_0_address << endl;

        auto xfer_circuit = make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(
                tree_height);

        input_note in[2];
        for (size_t i = 0; i < 2; i++) {
            auto idx = input_0_address + i;
            in[i].address = input_0_address + i;
            in[i].a_sk = coins[idx].a_sk;
            in[i].rho = coins[idx].rho;
            in[i].r = coins[idx].r;
            in[i].v = coins[idx].v;
            in[i].path = mt.path(idx);
        }

        output_note out[2];
        for (size_t i = 0; i < 2; i++) {
            auto idx = input_0_address + 2 + i;
            out[i].a_pk = prf_addr_a_pk<CommitmentHashT>(coins[idx].a_sk);
            out[i].rho = coins[idx].rho;
            out[i].r = coins[idx].r;
            out[i].v = coins[idx].v;
        }

        bit_vector mt_root = mt.root();
        FieldT callee("0");
        populate(xfer_circuit, tree_height, mt_root, in[0], in[1], out[0],
                 out[1], callee);
        print(xfer_circuit);
        ASSERT_FALSE(xfer_circuit.pb->is_satisfied());

        generate_witness(xfer_circuit);

        ASSERT_TRUE(xfer_circuit.pb->is_satisfied());

        for (size_t i = 0; i < 2; i++) {
            auto idx = input_0_address + 2 + i;
            auto a_pk = prf_addr_a_pk<CommitmentHashT>(coins[idx].a_sk);
            auto k = comm_r<CommitmentHashT>(a_pk, coins[idx].rho,
                                             coins[idx].r);
            auto cm = comm_s<CommitmentHashT>(k, uint64_to_bits(coins[idx].v));
            mt.add(cm);
        }



    }

    for (size_t address = 0; address < coins.size(); address++) {
        // WITHDRAWAL/"UNSHIELDING"

        cout << "Withdrawal address " << address << endl;

        coin c = coins[address];
        auto a_pk = prf_addr_a_pk<CommitmentHashT>(c.a_sk);
        cout << "a_pk  " << bits2hex(a_pk) << endl;
        auto k = comm_r<CommitmentHashT>(a_pk, c.rho, c.r);
        cout << "k  " << bits2hex(k) << endl;
        bit_vector v_bits = uint64_to_bits(c.v);
        auto cm = comm_s<CommitmentHashT>(k, v_bits);
        cout << "cm " << bits2hex(cm) << endl;
        bit_vector address_bits = int_to_bits<FieldT>(c.address, tree_height);
        string v_str = to_string(c.v);
        FieldT v = FieldT(v_str.c_str());

        // Generate recipient address
        uint32_t recipient = rand() % (uint32_t) exp2(20);

        // Generate proof
        auto wd_circuit = make_withdrawal_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(
                tree_height);
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.recipient_public) = recipient;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        wd_circuit.pb->val(*wd_circuit.recipient_private) = recipient;
        //ASSERT_FALSE(wd_circuit.pb->is_satisfied());
        wd_circuit.rt_packer->generate_r1cs_witness_from_bits();
        wd_circuit.v_packer->generate_r1cs_witness_from_packed();
        cout << "BEFORE" << endl;
        cout << "Root " << bits2hex(wd_circuit.rt_bits->get_digest()) << endl;
        cout << "a_sk          "
             << bits2hex(wd_circuit.a_sk_bits->get_bits(*wd_circuit.pb))
             << endl;
        cout << "rho          "
             << bits2hex(wd_circuit.rho_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "r          "
             << bits2hex(wd_circuit.r_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "v          "
             << bits2hex(wd_circuit.v_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "cm          "
             << bits2hex(wd_circuit.commitment_bits->get_digest()) << endl;
        cout << "Path" << endl;

        wd_circuit.note_gadget->generate_r1cs_witness();
        wd_circuit.sn_packer->generate_r1cs_witness_from_bits();

        cout << "AFTER " << address << endl;
        cout << "Root " << bits2hex(wd_circuit.rt_bits->get_digest()) << endl;
        cout << "a_sk          "
             << bits2hex(wd_circuit.a_sk_bits->get_bits(*wd_circuit.pb))
             << endl;
        cout << "rho          "
             << bits2hex(wd_circuit.rho_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "r          "
             << bits2hex(wd_circuit.r_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "v          "
             << bits2hex(wd_circuit.v_bits->get_bits(*wd_circuit.pb)) << endl;
        cout << "cm          "
             << bits2hex(wd_circuit.commitment_bits->get_digest()) << endl;
        cout << "Path" << endl;
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());

        // Set original inputs again to make sure nothing has been overwritten
        wd_circuit.rt_bits->generate_r1cs_witness(mt.root());
        wd_circuit.pb->val(*wd_circuit.v_packed) = v;
        wd_circuit.pb->val(*wd_circuit.ZERO) = FieldT::zero();
        wd_circuit.a_sk_bits->fill_with_bits(*wd_circuit.pb, c.a_sk);
        wd_circuit.rho_bits->fill_with_bits(*wd_circuit.pb, c.rho);
        wd_circuit.r_bits->fill_with_bits(*wd_circuit.pb, c.r);
        wd_circuit.address_bits->fill_with_bits(*wd_circuit.pb, address_bits);
        wd_circuit.path->generate_r1cs_witness(address, mt.path(address));
        // Circuit should still be satisfied
        ASSERT_TRUE(wd_circuit.pb->is_satisfied());
    }
}
