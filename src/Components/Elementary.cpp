#include "Elementary.hpp"
#include "AComponent.hpp"
#include "Logic.hpp"

namespace nts {
// And
AndComponent::AndComponent(const std::string &name) : AComponent(name) {}
nts::Tristate AndComponent::compute(size_t pin) {
  if (pin == 3)
    return getPinValue(1) && getPinValue(2);
  return nts::Undefined;
}


// Or
OrComponent::OrComponent(const std::string &name) : AComponent(name) {}
nts::Tristate OrComponent::compute(size_t pin) {
  if (pin == 3)
    return getPinValue(1) || getPinValue(2);
  return nts::Undefined;
}


// Xor
XorComponent::XorComponent(const std::string &name) : AComponent(name) {}
nts::Tristate XorComponent::compute(size_t pin) {
  if (pin == 3)
    return getPinValue(1) ^ getPinValue(2);
  return nts::Undefined;
}


// Not
NotComponent::NotComponent(const std::string &name) : AComponent(name) {}
nts::Tristate NotComponent::compute(size_t pin) {
  if (pin == 2)
    return !getPinValue(1);
  return nts::Undefined;
}
} // namespace nts
