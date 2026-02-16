#pragma once

#include "AComponent.hpp"

namespace nts {
// 4081
class C4081 : public AComponent {
public:
  C4081(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

// 4001
class C4001 : public AComponent {
public:
  C4001(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

// 4011
class C4011 : public AComponent {
public:
  C4011(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

// 4030
class C4030 : public AComponent {
public:
  C4030(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

// 4071
class C4071 : public AComponent {
public:
  C4071(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

// 4008
class C4008 : public AComponent {
public:
  C4008(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
} // namespace nts
