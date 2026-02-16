#pragma once

#include "AComponent.hpp"
#include "IComponent.hpp"
#include <cstddef>

namespace nts {
class C4008 : public AComponent {
public:
  C4008(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class C4512 : public AComponent {
  public:
    C4512(const std::string &name);
    nts::Tristate compute(std::size_t pin) override;
};

class C4514 : public AComponent {
  public:
    C4514(const std::string &name);
    nts::Tristate compute(std::size_t pin) override;
};
}
