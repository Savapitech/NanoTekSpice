#include <utility>

#include <iostream>

#include "AComponent.hpp"
#include "Error.hpp"
#include "IComponent.hpp"

namespace nts {
AComponent::AComponent(const std::string &name) : _name(name) {}

void AComponent::simulate(std::size_t) {}

nts::Tristate AComponent::compute(std::size_t) { return nts::Undefined; }

const std::string &AComponent::getName() const { return _name; }

void AComponent::setLink(std::size_t pin, nts::IComponent &other,
                         std::size_t otherPin) {
  _links[pin].push_back(std::make_pair(&other, otherPin));
}

void nts::AComponent::setValue(nts::Tristate value) {
  throw nts::ComponentError(
      "Cannot set value on a component that is not an input or a clock");
}

nts::Tristate AComponent::getPinValue(std::size_t pin) {
  auto it = _links.find(pin);
  if (it == _links.end())
    return nts::Undefined;
  for (auto &target : it->second) {
    AComponent *targetComp = dynamic_cast<AComponent *>(target.first);
    if (targetComp) {
      if (targetComp->_computingPins.count(target.second))
        continue;
      targetComp->_computingPins.insert(target.second);
      auto result = target.first->compute(target.second);
      targetComp->_computingPins.erase(target.second);
      if (result != nts::Undefined)
        return result;
    } else {
      auto result = target.first->compute(target.second);
      if (result != nts::Undefined)
        return result;
    }
  }
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
