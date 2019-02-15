#ifndef ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC
#define ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_TCC

template<typename FieldT>
WithdrawalCircuit<FieldT> make_withdrawal_circuit(size_t tree_height) {
    protoboard<FieldT> *pb = new protoboard<FieldT>();

    pb_variable_array<FieldT> *root_packed = new pb_variable_array<FieldT>();
    root_packed->allocate(*pb, 2, "root_packed");

    pb_variable<FieldT> *v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");

    pb_variable_array<FieldT> *sn_packed = new pb_variable_array<FieldT>();
    sn_packed->allocate(*pb, 2, "sn_packed");

    pb->set_input_sizes(5);

    pb_variable<FieldT> *ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    digest_variable<FieldT> *root_bits = new digest_variable<FieldT>(*pb, 256,
                                                                     "root_bits");

    pb_variable_array<FieldT> *address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    pb_variable_array<FieldT> *a_sk_bits = new pb_variable_array<FieldT>();
    a_sk_bits->allocate(*pb, 256, "a_sk_bits");

    auto a_pk_bits = make_shared<digest_variable<FieldT>>(*pb, 256,
                                                          "a_pk_bits");

    pb_variable_array<FieldT> *v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");

    pb_variable_array<FieldT> *rho_bits = new pb_variable_array<FieldT>();
    rho_bits->allocate(*pb, 256, "rho_bits");

    pb_variable_array<FieldT> *r_bits = new pb_variable_array<FieldT>();
    r_bits->allocate(*pb, 384, "r_bits");

    digest_variable<FieldT> *commitment_bits = new digest_variable<FieldT>(*pb,
                                                                           256,
                                                                           "commitment_bits");
    auto sn_bits = make_shared<digest_variable<FieldT>>(*pb, 256, "sn_bits");

    multipacking_gadget<FieldT> *root_packer = new multipacking_gadget<FieldT>(
            *pb, root_bits->bits, *root_packed, 128, "root_packer");
    packing_gadget<FieldT> *v_packer = new packing_gadget<FieldT>(*pb, *v_bits,
                                                                  *v_packed,
                                                                  "v_unpacker");
    multipacking_gadget<FieldT> *sn_packer = new multipacking_gadget<FieldT>(
            *pb, sn_bits->bits, *sn_packed, 128, "sn_packer");

    prf_addr_gadget<FieldT>* addr_gadget = new prf_addr_gadget<FieldT>(*pb, *ZERO,
                                                               *a_sk_bits,
                                                               a_pk_bits,
                                                               "prf_addr");

    cm_gadget<FieldT> *commitment_gadget = new cm_gadget<FieldT>(*pb, *ZERO, a_pk_bits->bits,
                                                   *rho_bits, *r_bits, *v_bits,
                                                   *commitment_bits);

    auto sn_gadget = new prf_sn_gadget<FieldT>(*pb, *ZERO, *a_sk_bits, *rho_bits, sn_bits, "prf_sn");

    typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;
    merkle_authentication_path_variable<FieldT, TwoToOneSHA256> *path = new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(
            *pb, tree_height, "merkle_authentication_path");
    merkle_tree_check_read_gadget<FieldT, TwoToOneSHA256> *mt_path_gadget = new merkle_tree_check_read_gadget<FieldT, TwoToOneSHA256>(
            *pb,
            tree_height,
            *address_bits,
            *commitment_bits,
            *root_bits,
            *path,
            ONE,
            "merkle_tree_check_read_gadget");

    pb->add_r1cs_constraint(
            r1cs_constraint<FieldT>(*ZERO, ONE, FieldT::zero()),
            "ZERO must equal zero");
    commitment_bits->generate_r1cs_constraints();
    root_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    sn_packer->generate_r1cs_constraints(true);
    addr_gadget->generate_r1cs_constraints();
    commitment_gadget->generate_r1cs_constraints();
    sn_gadget->generate_r1cs_constraints();
    mt_path_gadget->generate_r1cs_constraints();

    WithdrawalCircuit<FieldT> circuit{
            pb,
            root_packed,
            v_packed,
            sn_packed,
            root_bits,
            address_bits,
            a_sk_bits,
            a_pk_bits,
            v_bits,
            rho_bits,
            r_bits,
            commitment_bits,
            sn_bits,
            path,
            root_packer,
            v_packer,
            sn_packer,
            addr_gadget,
            commitment_gadget,
            sn_gadget,
            mt_path_gadget
    };

    return circuit;
}

#endif //ZKTRADE_CIRCUITRY_WITHDRAWAL_CIRCUIT_H
