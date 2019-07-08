#ifndef K0_EXAMPLECIRCUIT_HPP
#define K0_EXAMPLECIRCUIT_HPP

#include "circuitry/gadgets/ExampleGadget.hpp"
#include <memory>

namespace k0 {

template <typename FieldT> class ExampleCircuit {
  public:
    protoboard<FieldT> pb;
    pb_variable<FieldT> out;
    pb_variable<FieldT> x;
    unique_ptr<ExampleGadget<FieldT>> example_gadget;
    ExampleCircuit();
};

} // namespace k0

#include "ExampleCircuit.tcc"

#endif // K0_EXAMPLECIRCUIT_HPP
