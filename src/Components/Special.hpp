#pragma once

#include "AComponent.hpp"

namespace nts {
// Input
class InputComponent : public AComponent {
protected:
  nts::Tristate _value;
  nts::Tristate _nextValue;
  bool _valueChanged;

public:
  InputComponent(const std::string name);
  ~InputComponent() = default;

  void simulate(std::size_t tick) override;
  nts::Tristate compute(std::size_t pin) override;
  void setValue(nts::Tristate value) override;
};

// Output
class OutputComponent : public AComponent {
private:
  nts::Tristate _value;

public:
  OutputComponent(const std::string name);
  ~OutputComponent() = default;

  void simulate(std::size_t tick) override;
  nts::Tristate compute(std::size_t pin) override;
};

// True
class TrueComponent : public AComponent {
public:
  TrueComponent(const std::string name);
  nts::Tristate compute(std::size_t pin) override;
};

// False
class FalseComponent : public AComponent {
public:
  FalseComponent(const std::string name);
  nts::Tristate compute(std::size_t pin) override;
};

// Clock
class ClockComponent : public InputComponent {
public:
  ClockComponent(const std::string name);
  void simulate(std::size_t tick) override;
};
} // namespace nts
