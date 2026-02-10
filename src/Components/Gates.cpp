#include "Components/Gates.hpp"

namespace nts {
static nts::Tristate logicOr(nts::Tristate a, nts::Tristate b) {
  if (a == nts::True || b == nts::True)
    return nts::True;
  if (a == nts::False && b == nts::False)
    return nts::False;
  return nts::Undefined;
}

static nts::Tristate logicXor(nts::Tristate a, nts::Tristate b) {
  if (a == nts::Undefined || b == nts::Undefined)
    return nts::Undefined;
  return (a != b) ? nts::True : nts::False;
}

static nts::Tristate logicNot(nts::Tristate a) {
  if (a == nts::True)
    return nts::False;
  if (a == nts::False)
    return nts::True;
  return nts::Undefined;
}

static nts::Tristate logicAnd(nts::Tristate a, nts::Tristate b) {
  if (a == nts::False || b == nts::False)
    return nts::False;
  if (a == nts::True && b == nts::True)
    return nts::True;
  return nts::Undefined;
}

C4071::C4071(const std::string &name) : AComponent(name) {}
nts::Tristate C4071::compute(std::size_t pin) {
  if (pin == 3)
    return logicOr(getPinValue(1), getPinValue(2));
  if (pin == 4)
    return logicOr(getPinValue(5), getPinValue(6));
  if (pin == 10)
    return logicOr(getPinValue(8), getPinValue(9));
  if (pin == 11)
    return logicOr(getPinValue(12), getPinValue(13));
  return nts::Undefined;
}

C4030::C4030(const std::string &name) : AComponent(name) {}
nts::Tristate C4030::compute(std::size_t pin) {
  if (pin == 3)
    return logicXor(getPinValue(1), getPinValue(2));
  if (pin == 4)
    return logicXor(getPinValue(5), getPinValue(6));
  if (pin == 10)
    return logicXor(getPinValue(8), getPinValue(9));
  if (pin == 11)
    return logicXor(getPinValue(12), getPinValue(13));
  return nts::Undefined;
}

C4069::C4069(const std::string &name) : AComponent(name) {}
nts::Tristate C4069::compute(std::size_t pin) {
  if (pin == 2)
    return logicNot(getPinValue(1));
  if (pin == 4)
    return logicNot(getPinValue(3));
  if (pin == 6)
    return logicNot(getPinValue(5));
  if (pin == 8)
    return logicNot(getPinValue(9));
  if (pin == 10)
    return logicNot(getPinValue(11));
  if (pin == 12)
    return logicNot(getPinValue(13));
  return nts::Undefined;
}

C4001::C4001(const std::string &name) : AComponent(name) {}
nts::Tristate C4001::compute(std::size_t pin) {
  auto logicNor = [](nts::Tristate a, nts::Tristate b) {
    return logicNot(logicOr(a, b));
  };
  if (pin == 3)
    return logicNor(getPinValue(1), getPinValue(2));
  if (pin == 4)
    return logicNor(getPinValue(5), getPinValue(6));
  if (pin == 10)
    return logicNor(getPinValue(8), getPinValue(9));
  if (pin == 11)
    return logicNor(getPinValue(12), getPinValue(13));
  return nts::Undefined;
}

nts::Tristate C4011::compute(std::size_t pin) {
  auto logicNand = [](nts::Tristate a, nts::Tristate b) {
    return logicNot(logicAnd(a, b));
  };
  if (pin == 3)
    return logicNand(getPinValue(1), getPinValue(2));
  if (pin == 4)
    return logicNand(getPinValue(5), getPinValue(6));
  if (pin == 10)
    return logicNand(getPinValue(8), getPinValue(9));
  if (pin == 11)
    return logicNand(getPinValue(12), getPinValue(13));
  return nts::Undefined;
}
}
