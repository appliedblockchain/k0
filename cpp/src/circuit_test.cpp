#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/merkle_tree/merkle_tree_check_read_gadget.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
using namespace libsnark;

#include "printbits.hpp"
#include "serialization.hpp"
#include "circuits/NoteAdditionCircuit.hpp"
#include "circuits/NoteInclusionCircuit.hpp"
#include "util.hpp"
#include "MerkleTree.hpp"

using namespace std;
using namespace libff;
using namespace libsnark;

int main()
{
  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
  typedef sha256_two_to_one_hash_gadget<FieldT> HashT;
  default_r1cs_ppzksnark_pp::init_public_params();

//  const size_t digest_len = HashT::get_digest_len();

//  uint tree_height = 3;

  // protoboard<FieldT> pb;

  // pb_variable<FieldT> ZERO;
  //   ZERO.allocate(pb);
  //   pb.val(ZERO) = FieldT::zero();
    
  //   pb.add_r1cs_constraint(r1cs_constraint<FieldT>(ZERO, FieldT::one(), FieldT::zero()));

  //   digest_variable<FieldT> a_pk(pb, 256, "a_pk");
  //   digest_variable<FieldT> rho(pb, 256, "rho");
  //   digest_variable<FieldT> r(pb, 256, "r");

    

  // pb_variable_array<FieldT> v;
  // v.allocate(pb, 64, "v");



  // digest_variable<FieldT> result_digest(pb, 256, "result_digest");ß

  // shared_ptr<digest_variable<FieldT>> result = make_shared<digest_variable<FieldT>>(result_digest);
  //   printbits(result_digest.get_digest());

  //   //rho.generate_r1cs_witness(hex2bits("0xcafecafecafecafecafecafecafecafecafecafecafecafecafecafecafecafe"));
  //   a_pk.generate_r1cs_witness(hex2bits("0x0000000000000000000000000000000000000000000000000000000000000000"));
   //note_commitment_gadget<FieldT> ncg(*pb, ZERO, a_pk.bits, v, rho.bits, r.bits, result);

  //   a_pk.generate_r1cs_constraints();
  //   rho.generate_r1cs_constraints();
  //   r.generate_r1cs_constraints();
  // result_digest.generate_r1cs_constraints();
  // ncg.generate_r1cs_constraints();
    
  //   ncg.generate_r1cs_witness();
  //   printbits(r.get_digest());
  //   printbits(result_digest.get_digest());


    size_t tree_height = 4;
  MerkleTree mt(tree_height);
    
    for (int address = 0; address < 1; address++) {

  cout << "MT ROOT ";
  printbits(mt.root());


  NoteAdditionCircuit<FieldT, HashT> nac = make_note_addition_circuit<FieldT, HashT>(tree_height);

  nac.prev_root_bits->generate_r1cs_witness(mt.root());
  nac.prev_root_unpacker->generate_r1cs_witness_from_bits();

    nac.prev_path->generate_r1cs_witness(address, mt.path(address));
    


    nac.pb->val(*nac.address_packed) = address;
    nac.address_unpacker->generate_r1cs_witness_from_packed();
    
  cout << "setting v" << endl;
        auto value = FieldT("0");
        nac.pb->val(*nac.v_packed) = value;
  nac.v_unpacker->generate_r1cs_witness_from_packed();

  auto a_pk_bv = hex2bits("0xe0d3cf665fad7012e5ebbe984eb3e44a13b518daa0087dd8e647f97ad43f68c7");
  cout << "a_pk ";
  printbits(a_pk_bv);
  nac.a_pk_bits->fill_with_bits(*nac.pb, a_pk_bv);

  //auto rho_bv = random_bits();
  auto rho_bv = hex2bits("0xeecf10157fa8138cf3b8328e8fbff50e5284b65941178d51c72cf72dbc015b71");
  cout << "rho_bv ";
  printbits(rho_bv);
  nac.rho_bits->fill_with_bits(*nac.pb, rho_bv);

  auto r_bv = hex2bits("0xb7c5e33bcb0921c94363a7fbd526bc9ef97d896ec6e69b21ae4fce756907ae1c");
  cout << "r_bv";
  printbits(r_bv);
  nac.r_bits->fill_with_bits(*nac.pb, r_bv);

  nac.ncg->generate_r1cs_witness();
    
        auto commitment = nac.commitment_bits->get_digest();
    auto simulation_result = mt.simulate_add(commitment);
    bit_vector next_root_bv = get<1>(simulation_result);
    vector<bit_vector> next_path = get<2>(simulation_result);
    nac.next_path->generate_r1cs_witness(0, next_path);
    
    nac.next_root_bits->generate_r1cs_witness(next_root_bv);
    
  nac.mtcug->generate_r1cs_witness();

    nac.prev_root_unpacker->generate_r1cs_witness_from_bits();
    nac.commitment_unpacker->generate_r1cs_witness_from_bits();
    nac.next_root_unpacker->generate_r1cs_witness_from_bits();
  printbits(nac.commitment_bits->get_digest());
    
    cout << "address" << endl;
    auto address_bits = nac.address_bits->get_bits(*nac.pb);
    for (uint i = 0; i < 2; i++) {
        cout << address_bits[i] ? "1" : "0";
    }
    cout << endl;

    if (nac.pb->is_satisfied()) {
        cout << "SATISFIED" << endl;
    } else {
        cout << "NOT SATISFIED" << endl;
    }
        
        mt.add(commitment);

        NoteInclusionCircuit<FieldT, HashT> nic = make_note_inclusion_circuit<FieldT, HashT>(tree_height);
        nic.root_bits->generate_r1cs_witness(mt.root());
        nic.root_packer->generate_r1cs_witness_from_bits();
        nic.address_bits->fill_with_bits(*nic.pb, address_bits);
        nic.a_pk_bits->fill_with_bits(*nic.pb, a_pk_bv);
        nic.v_packer->generate_r1cs_witness_from_packed();
        nic.rho_bits->fill_with_bits(*nic.pb, rho_bv);
        nic.r_bits->fill_with_bits(*nic.pb, r_bv);
        nic.ncg->generate_r1cs_witness();
        
        if (nic.pb->is_satisfied()) {
            cout << "SATISFIED" << endl;
        } else {
            cout << "NOT SATISFIED" << endl;
        }
        
    }
  return 0;
}

