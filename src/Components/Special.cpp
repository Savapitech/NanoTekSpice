#include "Special.hpp"
#include "AComponent.hpp"
#include "IComponent.hpp"
#include "Logic.hpp"
#include <cstdint>

namespace nts {
// Input
InputComponent::InputComponent(const std::string name)
    : AComponent(name), _value(nts::Undefined), _nextValue(nts::Undefined),
      _valueChanged(false) {}

void InputComponent::simulate(std::size_t) {
  if (_valueChanged)
    _value = _nextValue;
  _valueChanged = false;
}

void InputComponent::setValue(nts::Tristate value) {
  _nextValue = value;
  _valueChanged = true;
}

nts::Tristate InputComponent::compute(std::size_t pin) {
  if (pin == 1)
    return _value;
  return nts::Undefined;
}

// Output
OutputComponent::OutputComponent(const std::string name)
    : AComponent(name), _value(nts::Undefined) {}

void OutputComponent::simulate(std::size_t) { _value = getPinValue(1); }

nts::Tristate OutputComponent::compute(std::size_t pin) {
  if (pin == 1)
    return _value;
  return nts::Undefined;
}

// True
TrueComponent::TrueComponent(const std::string name) : AComponent(name) {}
nts::Tristate TrueComponent::compute(std::size_t) { return nts::True; }

// False
FalseComponent::FalseComponent(const std::string name) : AComponent(name) {}
nts::Tristate FalseComponent::compute(std::size_t) { return nts::False; }

// Clock
ClockComponent::ClockComponent(const std::string name) : InputComponent(name) {}

void ClockComponent::simulate(std::size_t) {
  if (_valueChanged) {
    _value = _nextValue;
    _valueChanged = false;
  } else
    _value = !_value;
}

// Logger
LoggerComponent::LoggerComponent(const std::string &name)
    : AComponent(name), _lastClk(nts::Undefined) {}

nts::Tristate LoggerComponent::compute(std::size_t pin) {
  (void)pin;
  return nts::Undefined;
}

void LoggerComponent::simulate(std::size_t) {
  nts::Tristate clk = getPinValue(9);
  nts::Tristate inhibit = getPinValue(10);

  if (_lastClk != nts::True && clk == nts::True && inhibit == nts::False) {
    uint8_t byte = 0;
    for (std::size_t i = 0; i < 8; i++) {
      nts::Tristate bit = getPinValue(i + 1);
      if (bit == nts::Undefined)
        return;
      if (bit == nts::True)
        byte |= (1 << i);
    }
    std::ofstream file("./log.bin", std::ios::binary | std::ios::app);
    if (file.is_open()) {
      file.write(reinterpret_cast<const char *>(&byte), 1);
      file.flush();
    }
  }
  _lastClk = clk;
}
} // namespace nts
