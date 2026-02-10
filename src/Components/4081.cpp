#include "Components/4081.hpp"

namespace nts {
C4081::C4081(const std::string &name) : AComponent(name) {}

nts::Tristate C4081::compute(std::size_t pin) {
  auto logicAnd = [](nts::Tristate a, nts::Tristate b) {
    if (a == nts::False || b == nts::False)
      return nts::False;
    if (a == nts::True && b == nts::True)
      return nts::True;
    return nts::Undefined;
  };

  if (pin == 3)
    return logicAnd(getPinValue(1), getPinValue(2));
  if (pin == 4)
    return logicAnd(getPinValue(5), getPinValue(6));
  if (pin == 10)
    return logicAnd(getPinValue(8), getPinValue(9));
  if (pin == 11)
    return logicAnd(getPinValue(12), getPinValue(13));

  return nts::Undefined;
}
}
