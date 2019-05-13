#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <circuitry/TransferCircuit.hpp>
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/MTAdditionCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "definitions.hpp"
#include "setup.cpp"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Need exactly 4 arguments (circuit type, tree height, "
                  << "path to pk, path to vk)" << std::endl;
        return 1;
    }

    default_r1cs_ppzksnark_pp::init_public_params();

    string circuit_type = argv[1];
    size_t tree_height = stoi(argv[2]);

    // TODO Make circuit base class
    if (circuit_type.compare("commitment") == 0) {
        cout << "COMMITMENT" << endl;
        auto circuit = make_commitment_circuit<FieldT, CommitmentHashT>();
        auto cs = circuit.pb->get_constraint_system();
        setup(cs, argv[3], argv[4]);
    } else if (circuit_type.compare("addition") == 0) {
        cout << "ADDITION" << endl;
        auto circuit =
                make_mt_addition_circuit<FieldT, MerkleTreeHashT>(tree_height);
        setup(circuit.pb->get_constraint_system(), argv[3], argv[4]);
    } else if (circuit_type.compare("transfer") == 0) {
        cout << "TRANSFER" << endl;
        auto circuit =
                make_transfer_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(tree_height);
        setup(circuit.pb->get_constraint_system(), argv[3], argv[4]);
    } else if (circuit_type.compare("withdrawal") == 0) {
        cout << "WITHDRAWAL" << endl;
        auto circuit =make_withdrawal_circuit<FieldT, CommitmentHashT, MerkleTreeHashT>(tree_height);
        setup(circuit.pb->get_constraint_system(), argv[3], argv[4]);
    } else {
        std::cerr << "Invalid circuit type" << endl;
        return 1;
    }
    
    return 0;
}
