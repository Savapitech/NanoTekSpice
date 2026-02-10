#include "Components/Special.hpp"

namespace nts {
InputComponent::InputComponent(const std::string &name) : AComponent(name) {
  _value = nts::Undefined;
  _nextValue = nts::Undefined;
}

nts::Tristate InputComponent::compute(std::size_t pin) {
  if (pin == 1)
    return _value;
  return nts::Undefined;
}

void InputComponent::setValue(nts::Tristate value) { _nextValue = value; }

void InputComponent::simulate(std::size_t) {
  _value = _nextValue;
  _nextValue = nts::Undefined;
}

OutputComponent::OutputComponent(const std::string &name) : AComponent(name) {}

nts::Tristate OutputComponent::compute(std::size_t pin) {
  if (pin == 1)
    return getPinValue(1);
  return nts::Undefined;
}

TrueComponent::TrueComponent(const std::string &name) : AComponent(name) {}
nts::Tristate TrueComponent::compute(std::size_t) { return nts::True; }

FalseComponent::FalseComponent(const std::string &name) : AComponent(name) {}
nts::Tristate FalseComponent::compute(std::size_t) { return nts::False; }

ClockComponent::ClockComponent(const std::string &name)
    : InputComponent(name) {}

void ClockComponent::simulate(std::size_t) {
  if (_nextValue != nts::Undefined) {
    _value = _nextValue;
    _nextValue = nts::Undefined;
  } else if (_value == nts::True)
    _value = nts::False;
  else if (_value == nts::False)
    _value = nts::True;
}
} // namespace nts
