#pragma once

#include "AComponent.hpp"

namespace nts {
// 4081
class C4081 : public AComponent {
public:
  C4081(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class C4008 : public AComponent {
public:
  C4008(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
} // namespace nts
