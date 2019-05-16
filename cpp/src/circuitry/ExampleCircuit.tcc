#include "ExampleCircuit.hpp"

template<typename FieldT>
zktrade::ExampleCircuit<FieldT>::ExampleCircuit()
{
    out.allocate(pb);
    x.allocate(pb);
    pb.set_input_sizes(1);
    example_gadget = std::make_unique<ExampleGadget<FieldT>>(pb, out, x, "example_gadget");
    example_gadget->generate_r1cs_constraints();
}