#include "Elementary.hpp"
#include "AComponent.hpp"
#include "Logic.hpp"

namespace nts {
// Not
NotComponent::NotComponent(const std::string name) : AComponent(name) {}
nts::Tristate NotComponent::compute(size_t pin) {
  if (pin == 2)
    return !getPinValue(1);
  return nts::Undefined;
}
} // namespace nts
