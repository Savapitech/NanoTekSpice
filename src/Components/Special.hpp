#pragma once

#include "AComponent.hpp"

namespace nts {
class InputComponent : public AComponent {
public:
  InputComponent(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  void setValue(nts::Tristate value) override;
  void simulate(std::size_t tick) override;

protected:
  nts::Tristate _value;
  nts::Tristate _nextValue;
};

class OutputComponent : public AComponent {
public:
  OutputComponent(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class TrueComponent : public AComponent {
public:
  TrueComponent(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class FalseComponent : public AComponent {
public:
  FalseComponent(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class ClockComponent : public InputComponent {
public:
  ClockComponent(const std::string &name);
  void simulate(std::size_t tick) override;
};
}
