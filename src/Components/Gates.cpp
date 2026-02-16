#include "Gates.hpp"
#include "AComponent.hpp"
#include "IComponent.hpp"
#include "Logic.hpp"

namespace nts {
// C4081
C4081::C4081(const std::string &name) : AComponent(name) {}

nts::Tristate C4081::compute(std::size_t pin) {
  if (pin == 3)
    return getPinValue(1) && getPinValue(2);
  if (pin == 4)
    return getPinValue(5) && getPinValue(6);
  if (pin == 10)
    return getPinValue(8) && getPinValue(9);
  if (pin == 11)
    return getPinValue(12) && getPinValue(13);

  return nts::Undefined;
}

// C4001
C4001::C4001(const std::string &name) : AComponent(name) {}

nts::Tristate C4001::compute(std::size_t pin) {
  if (pin == 3)
    return !(getPinValue(1) || getPinValue(2));
  if (pin == 4)
    return !(getPinValue(5) || getPinValue(6));
  if (pin == 10)
    return !(getPinValue(8) || getPinValue(9));
  if (pin == 11)
    return !(getPinValue(12) || getPinValue(13));

  return nts::Undefined;
}

// C4011
C4011::C4011(const std::string &name) : AComponent(name) {}

nts::Tristate C4011::compute(std::size_t pin) {
  if (pin == 3)
    return !(getPinValue(1) && getPinValue(2));
  if (pin == 4)
    return !(getPinValue(5) && getPinValue(6));
  if (pin == 10)
    return !(getPinValue(8) && getPinValue(9));
  if (pin == 11)
    return !(getPinValue(12) && getPinValue(13));

  return nts::Undefined;
}

// C4030
C4030::C4030(const std::string &name) : AComponent(name) {}

nts::Tristate C4030::compute(std::size_t pin) {
  if (pin == 3)
    return getPinValue(1) ^ getPinValue(2);
  if (pin == 4)
    return getPinValue(5) ^ getPinValue(6);
  if (pin == 10)
    return getPinValue(8) ^ getPinValue(9);
  if (pin == 11)
    return getPinValue(12) ^ getPinValue(13);

  return nts::Undefined;
}

// C4071
C4071::C4071(const std::string &name) : AComponent(name) {}

nts::Tristate C4071::compute(std::size_t pin) {
  if (pin == 3)
    return getPinValue(1) || getPinValue(2);
  if (pin == 4)
    return getPinValue(5) || getPinValue(6);
  if (pin == 10)
    return getPinValue(8) || getPinValue(9);
  if (pin == 11)
    return getPinValue(12) || getPinValue(13);

  return nts::Undefined;
}

// 4008

C4008::C4008(const std::string &name) : AComponent(name) {};
nts::Tristate C4008::compute(std::size_t pin) {
  if (pin == 10)
    return simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).first;
  if (pin == 11) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    return simple_add(getPinValue(4), getPinValue(5), c1).first;
  }
  if (pin == 12) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    return simple_add(getPinValue(2), getPinValue(3), c2).first;
  }
  if (pin == 13) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    nts::Tristate c3 = simple_add(getPinValue(2), getPinValue(3), c2).second;
    return simple_add(getPinValue(15), getPinValue(1), c3).first;
  }
  if (pin == 14) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    nts::Tristate c3 = simple_add(getPinValue(2), getPinValue(3), c2).second;
    nts::Tristate c4 = simple_add(getPinValue(15), getPinValue(1), c3).second;
    return c4;
  }

  return nts::Undefined;
}
} // namespace nts
