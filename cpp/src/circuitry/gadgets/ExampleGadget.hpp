#pragma once

namespace zktrade {

template <typename FieldT> class ExampleGadget : public gadget<FieldT> {
  private:
    pb_variable<FieldT> sym_1;
    pb_variable<FieldT> y;
    pb_variable<FieldT> sym_2;

  public:
    const pb_variable<FieldT> out;
    const pb_variable<FieldT> x;
    ExampleGadget(protoboard<FieldT> &pb,
                  const pb_variable<FieldT> &out,
                  const pb_variable<FieldT> &x,
                  const std::string &annotation_prefix);
    void generate_r1cs_constraints();
    void generate_r1cs_witness();
};

} // namespace zktrade

#include "ExampleGadget.tcc"
