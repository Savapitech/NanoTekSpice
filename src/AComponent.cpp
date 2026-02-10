#include <utility>

#include "AComponent.hpp"
#include "Circuit.hpp"

namespace nts {
AComponent::AComponent(const std::string &name) : _name(name) {}

void AComponent::simulate(std::size_t) {}

nts::Tristate AComponent::compute(std::size_t) { return nts::Undefined; }

const std::string &AComponent::getName() const { return _name; }

void AComponent::setLink(std::size_t pin, nts::IComponent &other,
                         std::size_t otherPin) {
  _links[pin] = std::make_pair(&other, otherPin);
}

void nts::AComponent::setValue(nts::Tristate value) {
  throw std::runtime_error(
      "Cannot set value on a component that is not a input or a clock");
}

nts::Tristate AComponent::getPinValue(std::size_t pin) {
  if (_links.find(pin) != _links.end())
    return _links[pin].first->compute(_links[pin].second);
  return nts::Undefined;
}
} // namespace nts

std::ostream &operator<<(std::ostream &s, nts::Tristate v) {
  if (v == nts::True)
    return s << "1";
  if (v == nts::False)
    return s << "0";
  return s << "U";
}
