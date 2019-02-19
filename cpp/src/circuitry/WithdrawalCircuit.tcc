#ifndef ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC
#define ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC

template<typename FieldT>
WithdrawalCircuit<FieldT> make_withdrawal_circuit(size_t tree_height) {
    protoboard<FieldT> *pb = new protoboard<FieldT>();

    pb_variable_array<FieldT> *rt_packed = new pb_variable_array<FieldT>();
    rt_packed->allocate(*pb, 2, "rt_packed");

    pb_variable<FieldT> *v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");

    pb_variable_array<FieldT> *sn_packed = new pb_variable_array<FieldT>();
    sn_packed->allocate(*pb, 2, "sn_packed");

    pb_variable<FieldT> *recipient_public = new pb_variable<FieldT>();
    recipient_public->allocate(*pb, "recipient_public");

    pb->set_input_sizes(6);

    pb_variable<FieldT> *ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    digest_variable<FieldT> *rt_bits =
            new digest_variable<FieldT>( *pb, 256, "rt_bits");

    pb_variable_array<FieldT> *v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");

    pb_variable_array<FieldT> *a_sk_bits = new pb_variable_array<FieldT>();
    a_sk_bits->allocate(*pb, 256, "a_sk_bits");

    pb_variable_array<FieldT> *rho_bits = new pb_variable_array<FieldT>();
    rho_bits->allocate(*pb, 256, "rho_bits");

    pb_variable_array<FieldT> *r_bits = new pb_variable_array<FieldT>();
    r_bits->allocate(*pb, 384, "r_bits");

    pb_variable_array<FieldT> *address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;
    merkle_authentication_path_variable<FieldT, TwoToOneSHA256> *path =
            new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(
                    *pb, tree_height, "merkle_authentication_path");

    pb_variable<FieldT> *recipient_private = new pb_variable<FieldT>();
    recipient_private->allocate(*pb, "recipient_private");

    auto a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256, "a_pk_bits");

    digest_variable<FieldT> *commitment_bits =
            new digest_variable<FieldT>( *pb, 256, "commitment_bits");

    auto sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256, "sn_bits");

    multipacking_gadget<FieldT> *rt_packer =
            new multipacking_gadget<FieldT>(*pb, rt_bits->bits, *rt_packed, 128, "rt_packer");

    packing_gadget<FieldT> *v_packer =
            new packing_gadget<FieldT>( *pb, *v_bits, *v_packed, "v_packer");

    multipacking_gadget<FieldT> *sn_packer =
            new multipacking_gadget<FieldT>(*pb, sn_bits->bits, *sn_packed, 128, "sn_packer");

    prf_addr_gadget<FieldT>* addr_gadget = new prf_addr_gadget<FieldT>(*pb, *ZERO,
                                                               *a_sk_bits,
                                                               a_pk_bits,
                                                               "prf_addr");


    cm_gadget<FieldT> *commitment_gadget = new cm_gadget<FieldT>(*pb, *ZERO, a_pk_bits->bits,
                                                   *rho_bits, *r_bits, *v_bits,
                                                   *commitment_bits);

    auto sn_gadget = new prf_sn_gadget<FieldT>(*pb, *ZERO, *a_sk_bits, *rho_bits, sn_bits, "prf_sn");

    merkle_tree_check_read_gadget<FieldT, TwoToOneSHA256> *mt_path_gadget = new merkle_tree_check_read_gadget<FieldT, TwoToOneSHA256>(
            *pb,
            tree_height,
            *address_bits,
            *commitment_bits,
            *rt_bits,
            *path,
            ONE,
            "merkle_tree_check_read_gadget");


    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()),
            "ZERO must equal zero");

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*recipient_public, ONE, *recipient_private),
            "recipient_public must equal recipient_private");

    commitment_bits->generate_r1cs_constraints();
    a_pk_bits->generate_r1cs_constraints();

    rt_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    sn_packer->generate_r1cs_constraints(true);
    addr_gadget->generate_r1cs_constraints();
    commitment_gadget->generate_r1cs_constraints();
    sn_gadget->generate_r1cs_constraints();
    mt_path_gadget->generate_r1cs_constraints();

    WithdrawalCircuit<FieldT> circuit{
            pb,
            rt_packed,
            v_packed,
            sn_packed,
            recipient_public,
            ZERO,
            rt_bits,
            v_bits,
            a_sk_bits,
            rho_bits,
            r_bits,
            address_bits,
            path,
            recipient_private,
            a_pk_bits,
            commitment_bits,
            sn_bits,
            rt_packer,
            v_packer,
            sn_packer,
            addr_gadget,
            commitment_gadget,
            sn_gadget,
            mt_path_gadget
    };

    return circuit;
}

#endif //ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC
