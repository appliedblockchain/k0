#ifndef ZKTRADE_EXAMPLECIRCUIT_HPP
#define ZKTRADE_EXAMPLECIRCUIT_HPP

#include "circuitry/gadgets/ExampleGadget.hpp"
#include <memory>

namespace zktrade {

template <typename FieldT> class ExampleCircuit {
  public:
    protoboard<FieldT> pb;
    pb_variable<FieldT> out;
    pb_variable<FieldT> x;
    unique_ptr<ExampleGadget<FieldT>> example_gadget;
    ExampleCircuit();
};

} // namespace zktrade

#include "ExampleCircuit.tcc"

#endif // ZKTRADE_EXAMPLECIRCUIT_HPP