#include "Gates.hpp"
#include "Logic.hpp"

namespace nts {
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
} // namespace nts
