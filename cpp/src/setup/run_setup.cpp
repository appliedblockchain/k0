#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "circuitry/CommitmentCircuit.hpp"
#include "circuitry/WithdrawalCircuit.hpp"
#include "setup.cpp"

using namespace libff;
using namespace libsnark;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Need exactly 4 arguments (circuit type, tree height, "
                  << "path to pk, path to vk)" << std::endl;
        return 1;
    }

    typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
    default_r1cs_ppzksnark_pp::init_public_params();

    string circuit_type = argv[1];
    size_t tree_height = stoi(argv[2]);

    // TODO Make circuit base class
    if (circuit_type.compare("commitment") == 0) {
        cout << "COMMITMENT" << endl;
        CommitmentCircuit<FieldT> circuit = make_commitment_circuit<FieldT>();
        auto cs = circuit.pb->get_constraint_system();

    cout << "Num inputs:    : " << cs.num_inputs() << endl;
    cout << "Num variables  : " << cs.num_variables() << endl;
    cout << "Num constraints: " << cs.num_constraints() << endl;
        setup(cs, argv[3], argv[4]);
    } else if (circuit_type.compare("withdrawal") == 0) {
        cout << "WITHDRAWAL" << endl;
        WithdrawalCircuit<FieldT> circuit =
                make_withdrawal_circuit<FieldT>(tree_height);
        setup(circuit.pb->get_constraint_system(), argv[3], argv[4]);
    } else {
        std::cerr << "Invalid circuit type" << endl;
        return 1;
    }
    return 0;
}
