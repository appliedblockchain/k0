#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "AdditionCircuit.hpp"
#include "../../MerkleTree.hpp"
#include "../../util.hpp"

using namespace libff;
using namespace libsnark;
using namespace std;

int main() {
    typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
    typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
    default_r1cs_ppzksnark_pp::init_public_params();

    for (size_t tree_height = 1; tree_height < 4; tree_height++)
    {
        cout << endl << "Tree height " << tree_height << endl;
        AdditionCircuit<FieldT, HashT> circuit = make_addition_circuit<FieldT, HashT>(tree_height);
        MerkleTree mt(tree_height);
    
        for (uint address = 0; address < exp2(tree_height); address++)
        {
            cout << "Address " << address << endl;
            circuit.pb->val(*circuit.address_v) = FieldT(address);
            circuit.address_unpacker->generate_r1cs_witness_from_packed();
    
            auto prev_root_bv = mt.root();
            auto prev_root_field_elems = field_elements_from_bits<FieldT>(prev_root_bv);
            circuit.prev_root_va->fill_with_field_elements(*circuit.pb, prev_root_field_elems);
    
            bit_vector leaf_bv = random_bits();
            auto leaf_elems = field_elements_from_bits<FieldT>(leaf_bv);
            circuit.leaf_va->fill_with_field_elements(*circuit.pb, leaf_elems);

            auto simulation_result = mt.simulate_add(leaf_bv);
            bit_vector next_root_bv = get<1>(simulation_result);
            vector<bit_vector> path = get<2>(simulation_result);

            auto next_root_elems = field_elements_from_bits<FieldT>(next_root_bv);
            circuit.next_root_va->fill_with_field_elements(*circuit.pb, next_root_elems);

            circuit.path_var->generate_r1cs_witness(address, path);
            
            // cout << circuit.pb->primary_input() << endl;
            
            if (circuit.pb->is_satisfied())
            {
                cout << "SATISFIED 1" << endl;
            }
            else
            {
                cout << "NOT SATISFIED 1 (as expected)" << endl;
            }
            circuit.mtlap->generate_r1cs_witness();
            if (circuit.pb->is_satisfied())
            {
                cout << "SATISFIED 2 (as expected)" << endl;
            }
            else
            {
                cout << "NOT SATISFIED 2" << endl;
            }
            
            mt.add(leaf_bv);

        }
    }
    
}
