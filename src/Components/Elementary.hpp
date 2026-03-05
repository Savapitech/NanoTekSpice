#pragma once

#include "AComponent.hpp"
#include "Gates.hpp"

namespace nts {

template <typename Op> class BinaryGate : public AComponent {
public:
  BinaryGate(const std::string &name) : AComponent(name) {}
  nts::Tristate compute(std::size_t pin) override {
    if (pin == 3)
      return Op::apply(getPinValue(1), getPinValue(2));
    return nts::Undefined;
  }
};

using AndComponent = BinaryGate<AndOp>;
using OrComponent = BinaryGate<OrOp>;
using XorComponent = BinaryGate<XorOp>;

// Not (single input — different layout)
class NotComponent : public AComponent {
public:
  NotComponent(const std::string name);
  ~NotComponent() = default;
  nts::Tristate compute(size_t pin);
};
} // namespace nts
