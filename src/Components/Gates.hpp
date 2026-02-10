#pragma once

#include "AComponent.hpp"

namespace nts {
class C4071 : public AComponent {
public:
  C4071(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class C4030 : public AComponent {
public:
  C4030(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class C4069 : public AComponent {
public:
  C4069(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class C4001 : public AComponent {
public:
  C4001(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class C4011 : public AComponent {
public:
  C4011(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
} // namespace nts
