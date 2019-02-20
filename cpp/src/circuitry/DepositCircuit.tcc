template<typename FieldT>
DepositCircuit<FieldT>
zktrade::make_deposit_circuit(size_t tree_height) {

    typedef sha256_two_to_one_hash_gadget<FieldT> TwoToOneSHA256;

    auto pb = new protoboard<FieldT>();

    auto prev_root_packed = new pb_variable_array<FieldT>();
    prev_root_packed->allocate(*pb, 2, "prev_root_packed");

    auto address_packed = new pb_variable<FieldT>();
    address_packed->allocate(*pb, "address_packed");

    auto k_packed = new pb_variable_array<FieldT>();
    k_packed->allocate(*pb, 2, "k_packed");

    auto v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");

    auto cm_packed = new pb_variable_array<FieldT>();
    cm_packed->allocate(*pb, 2, "cm_packed");

    auto next_root_packed = new pb_variable_array<FieldT>();
    next_root_packed->allocate(*pb, 2, "next_root_packed");

    pb->set_input_sizes(10);

    merkle_authentication_path_variable<FieldT, TwoToOneSHA256>* path_var =
            new merkle_authentication_path_variable<FieldT, TwoToOneSHA256>(*pb, tree_height, "merkle_authentication_path");

    auto ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");

    auto prev_root_bits =
            new digest_variable<FieldT>(*pb, 256, "prev_root_bits");

    auto address_bits = new pb_variable_array<FieldT>();
    address_bits->allocate(*pb, tree_height, "address_bits");

    auto k_bits = new pb_variable_array<FieldT>();
    k_bits->allocate(*pb, 256, "k_bits");

    auto v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");

    auto cm_bits = new digest_variable<FieldT>(*pb, 256, "cm_bits");

    auto next_root_bits =
            new digest_variable<FieldT>(*pb, 256, "next_root_bits");

    auto prev_root_packer =
            new multipacking_gadget<FieldT>(*pb, prev_root_bits->bits, *prev_root_packed, 128, "prev_root_packer");

    auto address_packer =
            new packing_gadget<FieldT>(*pb, *address_bits, *address_packed, "address_packer");

    auto k_packer =
            new multipacking_gadget<FieldT>(*pb, *k_bits, *k_packed, 128, "k_packer");

    auto v_packer =
            new packing_gadget<FieldT>(*pb, *v_bits, *v_packed, "v_packer");

    auto cm_packer =
            new multipacking_gadget<FieldT>(*pb, cm_bits->bits, *cm_packed, 128, "cm_packer");

    auto next_root_packer =
            new multipacking_gadget<FieldT>(*pb, next_root_bits->bits, *next_root_packed, 128, "next_root_packer");

    auto commitment_gadget =
            new outer_commitment_gadget<FieldT>(*pb, *ZERO, *k_bits, *v_bits, *cm_bits, "outer_commitment_gadget");

    MTLeafAddition<FieldT, TwoToOneSHA256>* mt_addition_gadget =
            new MTLeafAddition<FieldT, TwoToOneSHA256>(*pb,
                                          tree_height,
                                          *ZERO,
                                          *address_bits,
                                          *prev_root_bits,
                                          *cm_bits,
                                          *next_root_bits,
                                          *path_var);

    // TODO add ZERO constraint (ZERO must be zero) (?)
    prev_root_bits->generate_r1cs_constraints();
    cm_bits->generate_r1cs_constraints();
    next_root_bits->generate_r1cs_constraints();
    prev_root_packer->generate_r1cs_constraints(true);
    k_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    cm_packer->generate_r1cs_constraints(true);
    next_root_packer->generate_r1cs_constraints(true);
    commitment_gadget->generate_r1cs_constraints();
    mt_addition_gadget->generate_r1cs_constraints();

    return zktrade::DepositCircuit<FieldT>{
            pb,
            prev_root_packed,
            address_packed,
            k_packed,
            v_packed,
            cm_packed,
            next_root_packed,
            path_var,
            ZERO,
            prev_root_bits,
            address_bits,
            k_bits,
            v_bits,
            cm_bits,
            next_root_bits,
            prev_root_packer,
            address_packer,
            k_packer,
            v_packer,
            cm_packer,
            next_root_packer,
            commitment_gadget,
            mt_addition_gadget
    };
}