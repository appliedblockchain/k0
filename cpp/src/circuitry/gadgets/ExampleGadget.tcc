namespace k0 {

template <typename FieldT>
ExampleGadget<FieldT>::ExampleGadget(protoboard<FieldT> &pb,
                                     const pb_variable<FieldT> &out,
                                     const pb_variable<FieldT> &x,
                                     const std::string &annotation_prefix)
    : gadget<FieldT>(pb, annotation_prefix), out(out), x(x) {
    sym_1.allocate(pb, "sym_1");
    y.allocate(pb, "y");
    sym_2.allocate(pb, "sym_2");
}

template <typename FieldT>
void ExampleGadget<FieldT>::generate_r1cs_constraints() {
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(x, x, sym_1),
                                 "x*x=sym_1");
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_1, x, y),
                                 "sym_1*x=y");
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(y + x, 1, sym_2),
                                 "(y+x)*1=sym_2");
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_2 + 5, 1, out),
                                 "(sym_2+5)*1=out");
}

template <typename FieldT> void ExampleGadget<FieldT>::generate_r1cs_witness() {
    this->pb.val(sym_1) = this->pb.val(x) * this->pb.val(x);
    this->pb.val(y) = this->pb.val(sym_1) * this->pb.val(x);
    this->pb.val(sym_2) = this->pb.val(y) + this->pb.val(x);
    this->pb.val(out) = this->pb.val(sym_2) + 5;
}

} // namespace k0
