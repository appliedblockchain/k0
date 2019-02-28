#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>

template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
TransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
zktrade::make_transfer_circuit(size_t tree_height) {
    auto pb = new protoboard<FieldT>();

    auto rt_packed = new pb_variable_array<FieldT>();
    rt_packed->allocate(*pb, 2, "rt_packed");

    vector<pb_variable_array<FieldT> *> sn_in_packed_vec(2);
    vector<pb_variable_array<FieldT> *> cm_out_packed_vec(2);

    for (int i = 0; i < 2; i++) {
        sn_in_packed_vec[i] = new pb_variable_array<FieldT>();
        sn_in_packed_vec[i]->allocate(*pb, 2,
                                      "sn_in_" + to_string(i) + "_packed");
    }

    for (int i = 0; i < 2; i++) {
        cm_out_packed_vec[i] = new pb_variable_array<FieldT>();
        cm_out_packed_vec[i]->allocate(*pb, 2,
                                       "cm_out_" + to_string(i) + "_packed");
    }

    pb->set_input_sizes(6);

    auto ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    auto rt_bits = new digest_variable<FieldT>(*pb, 256, "rt_bits");
    rt_bits->generate_r1cs_constraints();

    vector<pb_variable_array<FieldT> *> a_sk_in_bits_vec(2);
    vector<shared_ptr<digest_variable<FieldT>>> a_pk_in_bits_vec(2);
    vector<pb_variable_array<FieldT> *> rho_in_bits_vec(2);
    vector<pb_variable_array<FieldT> *> r_in_bits_vec(2);
    vector<pb_variable_array<FieldT> *> v_in_bits_vec(2);
    vector<digest_variable<FieldT> *> cm_in_bits_vec(2);
    vector<shared_ptr<digest_variable<FieldT>>> sn_in_bits_vec(2);
    vector<pb_variable_array<FieldT> *> address_in_bits_vec(2);
    vector<merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *> path_in_vec(2);
    vector<multipacking_gadget<FieldT> *> sn_in_packer(2);
    vector<input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT> *> input_note_vec(
            2);

    for (int i = 0; i < 2; i++) {
        a_sk_in_bits_vec[i] = new pb_variable_array<FieldT>();
        a_sk_in_bits_vec[i]->allocate(*pb, 256,
                                  "a_sk_in_" + to_string(i) + "_bits");

        a_pk_in_bits_vec[i] = make_shared<digest_variable<FieldT>>(
                *pb, 256, "a_pk_in_" + to_string(i) + "_bits");
        a_pk_in_bits_vec[i]->generate_r1cs_constraints();

        rho_in_bits_vec[i] = new pb_variable_array<FieldT>();
        rho_in_bits_vec[i]->allocate(*pb, 256, "rho_in_" + to_string(i) + "_bits");

        r_in_bits_vec[i] = new pb_variable_array<FieldT>();
        r_in_bits_vec[i]->allocate(*pb, 384, "r_in_" + to_string(i) + "_bits");

        v_in_bits_vec[i] = new pb_variable_array<FieldT>();
        v_in_bits_vec[i]->allocate(*pb, 64, "v_in_" + to_string(i) + "_bits");

        cm_in_bits_vec[i] = new digest_variable<FieldT>(
                *pb, 256, "cm_in_" + to_string(i) + "_bits");
        cm_in_bits_vec[i]->generate_r1cs_constraints();

        sn_in_bits_vec[i] = make_shared<digest_variable<FieldT>>(
                *pb, 256, "sn_in_" + to_string(i) + "_bits");
        sn_in_bits_vec[i]->generate_r1cs_constraints();

        address_in_bits_vec[i] = new pb_variable_array<FieldT>();
        address_in_bits_vec[i]->allocate(
                *pb, tree_height, "address_in_" + to_string(i) + "_bits");

        path_in_vec[i] =
                new merkle_authentication_path_variable<FieldT, MerkleTreeHashT>(
                        *pb, tree_height, "path_in_" + to_string(i));

        path_in_vec[i]->generate_r1cs_constraints();

        sn_in_packer[i] = new multipacking_gadget<FieldT>(
                *pb, sn_in_bits_vec[i]->bits, *sn_in_packed_vec[i], 128,
                "sn_in_" + to_string(i) + "_packer");
        sn_in_packer[i]->generate_r1cs_constraints(true);

        input_note_vec[i] = new input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>(
                *pb);
        input_note_vec[i]->generate_r1cs_constraints();
    }

    vector<pb_variable_array<FieldT> *> a_pk_out_bits_vec(2);
    vector<pb_variable_array<FieldT> *> rho_out_bits_vec(2);
    vector<pb_variable_array<FieldT> *> r_out_bits_vec(2);
    vector<pb_variable_array<FieldT> *> v_out_bits_vec(2);
    vector<digest_variable<FieldT> *> cm_out_bits_vec(2);
    vector<cm_gadget<FieldT, CommitmentHashT> *> cm_out_gadget_vec(2);

    for (int i = 0; i < 2; i++) {
        a_pk_out_bits_vec[i] = new pb_variable_array<FieldT>();
        a_pk_out_bits_vec[i]->allocate(*pb, 256,
                                   "a_pk_out_" + to_string(i) + "_bits");

        rho_out_bits_vec[i] = new pb_variable_array<FieldT>();
        rho_out_bits_vec[i]->allocate(*pb, 256,
                                  "rho_out_" + to_string(i) + "_bits");

        r_out_bits_vec[i] = new pb_variable_array<FieldT>();
        r_out_bits_vec[i]->allocate(*pb, 384, "r_out_" + to_string(i) + "_bits");

        v_out_bits_vec[i] = new pb_variable_array<FieldT>();
        v_out_bits_vec[i]->allocate(*pb, 64, "v_out_" + to_string(i) + "_bits");

        cm_out_bits_vec[i] = new digest_variable<FieldT>(
                *pb, 256, "cm_out_" + to_string(i) + "_bits");
        cm_out_bits_vec[i]->generate_r1cs_constraints();

        cm_out_gadget_vec[i] = new cm_gadget<FieldT, CommitmentHashT>(
                *pb, *ZERO, *a_pk_out_bits_vec[i], *rho_out_bits_vec[i],
                *r_out_bits_vec[i], *v_out_bits_vec[i],
                *cm_out_bits_vec[i]);
        cm_out_gadget_vec[i]->generate_r1cs_constraints();
    }

    auto rt_packer = new multipacking_gadget<FieldT>(
            *pb, rt_bits->bits, *rt_packed, 128, "rt_packer");

    generate_r1cs_equals_const_constraint<FieldT>(
            *pb, *ZERO, FieldT::zero(), "ZERO must be zero");

    return {
            pb,
            rt_packed,
            sn_in_packed_vec,
            cm_out_packed_vec,
            ZERO,
            rt_bits,
            a_sk_in_bits_vec,
            a_pk_in_bits_vec,
            rho_in_bits_vec,
            r_in_bits_vec,
            v_in_bits_vec,
            cm_in_bits_vec,
            sn_in_bits_vec,
            address_in_bits_vec,
            path_in_vec,
            sn_in_packer,
            input_note_vec,
            a_pk_out_bits_vec,
            rho_out_bits_vec,
            r_out_bits_vec,
            v_out_bits_vec,
            cm_out_bits_vec,
            cm_out_gadget_vec,
            rt_packer
    };
}
