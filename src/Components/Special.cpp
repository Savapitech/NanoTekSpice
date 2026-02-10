#include "Special.hpp"
#include "AComponent.hpp"

namespace nts {
// Input
InputComponent::InputComponent(const std::string &name)
    : AComponent(name), _value(nts::Undefined), _nextValue(nts::Undefined),
      _valueChanged(false) {}

void InputComponent::simulate(std::size_t) {
  if (_valueChanged)
    _value = _nextValue;
}

void InputComponent::setValue(nts::Tristate value) { _nextValue = value; }

nts::Tristate InputComponent::compute(std::size_t pin) {
  if (pin == 1)
    return _value;
  return nts::Undefined;
}

// Output
OutputComponent::OutputComponent(const std::string &name)
    : AComponent(name), _value(nts::Undefined) {}

void OutputComponent::simulate(std::size_t) { _value = getPinValue(1); }

nts::Tristate OutputComponent::compute(std::size_t pin) {
  if (pin == 1)
    return _value;
  return nts::Undefined;
}

// True
TrueComponent::TrueComponent(const std::string &name) : AComponent(name) {}
nts::Tristate TrueComponent::compute(std::size_t) { return nts::True; }

// False
FalseComponent::FalseComponent(const std::string &name) : AComponent(name) {}
nts::Tristate FalseComponent::compute(std::size_t) { return nts::False; }

// Clock
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
