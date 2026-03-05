#include "Gates.hpp"
#include "AComponent.hpp"
#include "IComponent.hpp"
#include "Logic.hpp"

namespace nts {
// 4069
C4069::C4069(const std::string &name) : AComponent(name) {};
nts::Tristate C4069::compute(std::size_t pin) {
  if (pin == 2)
    return !getPinValue(1);
  if (pin == 4)
    return !getPinValue(3);
  if (pin == 6)
    return !getPinValue(5);
  if (pin == 8)
    return !getPinValue(9);
  if (pin == 10)
    return !getPinValue(11);
  if (pin == 12)
    return !getPinValue(13);

  return nts::Undefined;
}
} // namespace nts
