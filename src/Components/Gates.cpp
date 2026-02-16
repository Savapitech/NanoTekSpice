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
} // namespace nts
