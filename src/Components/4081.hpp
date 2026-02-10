#pragma once

#include "AComponent.hpp"

namespace nts {
class C4081 : public AComponent {
public:
  C4081(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
} // namespace nts
