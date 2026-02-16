#pragma once

#include "AComponent.hpp"

namespace nts {
class C4008 : public AComponent {
public:
  C4008(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
}
