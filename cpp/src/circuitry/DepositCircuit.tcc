template<typename FieldT>
DepositCircuit<FieldT>
zktrade::make_deposit_circuit() {
    auto pb = new protoboard<FieldT>();
    auto k_packed = new pb_variable_array<FieldT>();
    k_packed->allocate(*pb, 2, "k_packed");
    auto v_packed = new pb_variable<FieldT>();
    v_packed->allocate(*pb, "v_packed");
    auto cm_packed = new pb_variable_array<FieldT>();
    cm_packed->allocate(*pb, 2, "cm_packed");
    auto ZERO = new pb_variable<FieldT>();
    ZERO->allocate(*pb, "ZERO");
    auto k_bits = new pb_variable_array<FieldT>();
    k_bits->allocate(*pb, 256, "k_bits");
    auto v_bits = new pb_variable_array<FieldT>();
    v_bits->allocate(*pb, 64, "v_bits");
    auto cm_bits = new digest_variable<FieldT>(*pb, 256, "cm_bits");
    auto k_packer = new multipacking_gadget<FieldT>(*pb, *k_bits, *k_packed,
                                                    128,
                                                    "k_packer");
    auto v_packer = new packing_gadget<FieldT>(*pb, *v_bits, *v_packed,
                                               "v_packer");
    auto cm_packer = new multipacking_gadget<FieldT>(*pb, cm_bits->bits,
                                                     *cm_packed, 128,
                                                     "cm_packer");
    auto ocmg = new outer_commitment_gadget<FieldT>(*pb, *ZERO, *k_bits,
                                                    *v_bits,
                                                    *cm_bits,
                                                    "outer_note_commitment_gadget");

    // TODO add ZERO constraint (ZERO must be zero) (?)
    cm_bits->generate_r1cs_constraints();
    k_packer->generate_r1cs_constraints(true);
    v_packer->generate_r1cs_constraints(true);
    cm_packer->generate_r1cs_constraints(true);
    ocmg->generate_r1cs_constraints();

    return zktrade::DepositCircuit<FieldT>{
            pb, k_packed, v_packed, cm_packed, ZERO, k_bits, v_bits, cm_bits,
            k_packer, v_packer, cm_packer, ocmg
    };
}