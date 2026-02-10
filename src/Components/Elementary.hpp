#pragma once

#include "AComponent.hpp"

namespace nts {
// And
class AndComponent : public AComponent {
public:
  AndComponent(const std::string name);
  ~AndComponent() = default;
  nts::Tristate compute(size_t pin);
};

// Or
class OrComponent : public AComponent {
public:
  OrComponent(const std::string name);
  ~OrComponent() = default;
  nts::Tristate compute(size_t pin);
};

// Xor
class XorComponent : public AComponent {
public:
  XorComponent(const std::string name);
  ~XorComponent() = default;
  nts::Tristate compute(size_t pin);
};

// Not
class NotComponent : public AComponent {
public:
  NotComponent(const std::string name);
  ~NotComponent() = default;
  nts::Tristate compute(size_t pin);
};
} // namespace nts
