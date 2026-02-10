#include "Components/Elementary.hpp"
#include "IComponent.hpp"

namespace nts {

nts::Tristate logicOr(nts::Tristate a, nts::Tristate b) {
  if (a == nts::True || b == nts::True)
    return nts::True;
  if (a == nts::False && b == nts::False)
    return nts::False;
  return nts::Undefined;
}

nts::Tristate logicXor(nts::Tristate a, nts::Tristate b) {
  if (a == nts::Undefined || b == nts::Undefined)
    return nts::Undefined;
  return (a != b) ? nts::True : nts::False;
}

nts::Tristate logicNot(nts::Tristate a) {
  if (a == nts::True)
    return nts::False;
  if (a == nts::False)
    return nts::True;
  return nts::Undefined;
}

nts::Tristate logicAnd(nts::Tristate a, nts::Tristate b) {
  if (a == nts::False || b == nts::False)
    return nts::False;
  if (a == nts::True && b == nts::True)
    return nts::True;
  return nts::Undefined;
}

nts::Tristate logicNor(nts::Tristate a, nts::Tristate b) {
  return logicNot(logicOr(a, b));
};

nts::Tristate logicNand(nts::Tristate a, nts::Tristate b) {
  return logicNot(logicAnd(a, b));
};

Or::Or(const std::string &name) : AComponent(name) {}
nts::Tristate Or::compute(std::size_t pin) {
  return logicOr(getPinValue(1), getPinValue(2));
}

And::And(const std::string &name) : AComponent(name) {}
nts::Tristate And::compute(std::size_t pin) {
  return logicAnd(getPinValue(1), getPinValue(2));
}

Xor::Xor(const std::string &name) : AComponent(name) {}
nts::Tristate Xor::compute(std::size_t pin) {
  return logicXor(getPinValue(1), getPinValue(2));
}

Not::Not(const std::string &name) : AComponent(name) {}
nts::Tristate Not::compute(std::size_t pin) { return logicNot(getPinValue(1)); }
} // namespace nts
