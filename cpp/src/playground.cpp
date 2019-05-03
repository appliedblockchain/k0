#include "definitions.hpp"
#include "circuitry/ExampleCircuit.hpp"

using namespace std;
using namespace libff;
using namespace libsnark;

int main()
{
    default_r1cs_ppzksnark_pp::init_public_params();
    zktrade::ExampleCircuit<FieldT> circuit;
    circuit.pb.val(circuit.x) = 5;
    circuit.example_gadget->generate_r1cs_witness();
    if (circuit.pb.is_satisfied()) {
        cout << "SATISFIED" << endl;
    } else {
        cout << "NOT SATISFIED" << endl;
    }
    cout << circuit.pb.val(circuit.out) << endl;
    return 0;
}

