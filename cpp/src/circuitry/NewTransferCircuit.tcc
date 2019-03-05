#ifndef ZKTRADE_NEWTRANSFERCIRCUIT_TCC
#define ZKTRADE_NEWTRANSFERCIRCUIT_TCC


template<typename FieldT, typename CommitmentHashT, typename MerkleTreeHashT>
NewTransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT>
zktrade::make_new_transfer_circuit(size_t tree_height) {
    protoboard<FieldT> *pb = new protoboard<FieldT>();

    pb_variable_array<FieldT> *rt_packed = new pb_variable_array<FieldT>();
    rt_packed->allocate(*pb, 2, "rt_packed");

    pb_variable_array<FieldT> *in_0_sn_packed = new pb_variable_array<FieldT>();
    in_0_sn_packed->allocate(*pb, 2, "in_0_sn_packed");

    pb_variable_array<FieldT> *in_1_sn_packed = new pb_variable_array<FieldT>();
    in_1_sn_packed->allocate(*pb, 2, "in_1_sn_packed");

    pb->set_input_sizes(6);

    pb_variable<FieldT> *ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    digest_variable<FieldT> *rt_bits =
            new digest_variable<FieldT>(*pb, 256, "rt_bits");


    pb_variable_array<FieldT> *in_0_v_bits = new pb_variable_array<FieldT>();
    in_0_v_bits->allocate(*pb, 64, "in_0_v_bits");

    pb_variable_array<FieldT> *in_0_a_sk_bits = new pb_variable_array<FieldT>();
    in_0_a_sk_bits->allocate(*pb, 256, "in_0_a_sk_bits");

    pb_variable_array<FieldT> *in_0_rho_bits = new pb_variable_array<FieldT>();
    in_0_rho_bits->allocate(*pb, 256, "in_0_rho_bits");

    pb_variable_array<FieldT> *in_0_r_bits = new pb_variable_array<FieldT>();
    in_0_r_bits->allocate(*pb, 384, "in_0_r_bits");

    pb_variable_array<FieldT> *in_0_address_bits = new pb_variable_array<FieldT>();
    in_0_address_bits->allocate(*pb, tree_height, "address_bits");

    merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_0_path =
            new merkle_authentication_path_variable<FieldT, MerkleTreeHashT>(
                    *pb, tree_height, "in_0_path");

    auto in_0_a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                               "a_pk_bits");

    digest_variable<FieldT> *in_0_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "in_0_cm_bits");

    auto in_0_sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                             "in_0_sn_bits");


    pb_variable_array<FieldT> *in_1_v_bits = new pb_variable_array<FieldT>();
    in_1_v_bits->allocate(*pb, 64, "in_1_v_bits");

    pb_variable_array<FieldT> *in_1_a_sk_bits = new pb_variable_array<FieldT>();
    in_1_a_sk_bits->allocate(*pb, 256, "in_1_a_sk_bits");

    pb_variable_array<FieldT> *in_1_rho_bits = new pb_variable_array<FieldT>();
    in_1_rho_bits->allocate(*pb, 256, "in_1_rho_bits");

    pb_variable_array<FieldT> *in_1_r_bits = new pb_variable_array<FieldT>();
    in_1_r_bits->allocate(*pb, 384, "in_1_r_bits");

    pb_variable_array<FieldT> *in_1_address_bits = new pb_variable_array<FieldT>();
    in_1_address_bits->allocate(*pb, tree_height, "address_bits");

    merkle_authentication_path_variable<FieldT, MerkleTreeHashT> *in_1_path =
            new merkle_authentication_path_variable<FieldT, MerkleTreeHashT>(
                    *pb, tree_height, "in_1_path");

    auto in_1_a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                               "a_pk_bits");

    digest_variable<FieldT> *in_1_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "in_1_cm_bits");

    auto in_1_sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                             "in_1_sn_bits");


    pb_variable_array<FieldT> *out_0_v_bits = new pb_variable_array<FieldT>();
    out_0_v_bits->allocate(*pb, 64, "out_0_v_bits");

    pb_variable_array<FieldT> *out_0_a_pk_bits = new pb_variable_array<FieldT>();
    out_0_a_pk_bits->allocate(*pb, 256, "out_0_a_pk_bits");

    pb_variable_array<FieldT> *out_0_rho_bits = new pb_variable_array<FieldT>();
    out_0_rho_bits->allocate(*pb, 256, "out_0_rho_bits");

    pb_variable_array<FieldT> *out_0_r_bits = new pb_variable_array<FieldT>();
    out_0_r_bits->allocate(*pb, 384, "out_0_r_bits");

    digest_variable<FieldT> *out_0_cm_bits =
            new digest_variable<FieldT>(*pb, 256, "out_0_cm_bits");


    multipacking_gadget<FieldT> *rt_packer =
            new multipacking_gadget<FieldT>(
                    *pb, rt_bits->bits, *rt_packed, 128, "rt_packer");

    multipacking_gadget<FieldT> *in_0_sn_packer =
            new multipacking_gadget<FieldT>(
                    *pb, in_0_sn_bits->bits, *in_0_sn_packed, 128, "sn_packer");

    multipacking_gadget<FieldT> *in_1_sn_packer =
            new multipacking_gadget<FieldT>(
                    *pb, in_1_sn_bits->bits, *in_1_sn_packed, 128, "sn_packer");

    auto in_0_note_gadget =
            new input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>(
                    tree_height,
                    *pb,
                    *ZERO,
                    *rt_bits,
                    *in_0_v_bits,
                    *in_0_a_sk_bits,
                    *in_0_rho_bits,
                    *in_0_r_bits,
                    *in_0_address_bits,
                    *in_0_path,
                    in_0_a_pk_bits,
                    *in_0_cm_bits,
                    in_0_sn_bits,
                    "in_0_note_gadget");

    auto in_1_note_gadget =
            new input_note_gadget<FieldT, CommitmentHashT, MerkleTreeHashT>(
                    tree_height,
                    *pb,
                    *ZERO,
                    *rt_bits,
                    *in_1_v_bits,
                    *in_1_a_sk_bits,
                    *in_1_rho_bits,
                    *in_1_r_bits,
                    *in_1_address_bits,
                    *in_1_path,
                    in_1_a_pk_bits,
                    *in_1_cm_bits,
                    in_1_sn_bits,
                    "in_1_note_gadget");

    auto out_0_cm_gadget = new cm_full_gadget<FieldT, CommitmentHashT>(
            *pb,
            *ZERO,
            *out_0_a_pk_bits,
            *out_0_rho_bits,
            *out_0_r_bits,
            *out_0_v_bits,
            *out_0_cm_bits,
            "out_0_cm_gadget");

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()),
            "ZERO must equal zero");

    in_0_path->generate_r1cs_constraints();
    in_0_a_pk_bits->generate_r1cs_constraints();
    in_0_cm_bits->generate_r1cs_constraints();
    in_1_path->generate_r1cs_constraints();
    in_1_a_pk_bits->generate_r1cs_constraints();
    in_1_cm_bits->generate_r1cs_constraints();


    rt_packer->generate_r1cs_constraints(true);
    in_0_sn_packer->generate_r1cs_constraints(true);
    in_0_note_gadget->generate_r1cs_constraints();
    in_1_sn_packer->generate_r1cs_constraints(true);
    in_1_note_gadget->generate_r1cs_constraints();
    out_0_cm_gadget->generate_r1cs_constraints();

    NewTransferCircuit<FieldT, CommitmentHashT, MerkleTreeHashT> circuit{
            pb,
            rt_packed,
            in_0_sn_packed,
            in_1_sn_packed,

            ZERO,
            rt_bits,

            in_0_v_bits,
            in_0_a_sk_bits,
            in_0_rho_bits,
            in_0_r_bits,
            in_0_address_bits,
            in_0_path,
            in_0_a_pk_bits,
            in_0_cm_bits,
            in_0_sn_bits,

            in_1_v_bits,
            in_1_a_sk_bits,
            in_1_rho_bits,
            in_1_r_bits,
            in_1_address_bits,
            in_1_path,
            in_1_a_pk_bits,
            in_1_cm_bits,
            in_1_sn_bits,

            out_0_v_bits,
            out_0_a_pk_bits,
            out_0_rho_bits,
            out_0_r_bits,
            out_0_cm_bits,

            rt_packer,
            in_0_sn_packer,
            in_1_sn_packer,
            in_0_note_gadget,
            in_1_note_gadget,
            out_0_cm_gadget
    };

    return circuit;
}

#endif //ZKTRADE_NEWTRANSFERCIRCUIT_TCC
