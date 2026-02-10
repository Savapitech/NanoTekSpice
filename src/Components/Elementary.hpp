#pragma once

#include "AComponent.hpp"

namespace nts {

nts::Tristate logicOr(nts::Tristate a, nts::Tristate b);
nts::Tristate logicXor(nts::Tristate a, nts::Tristate b);
nts::Tristate logicNot(nts::Tristate a);
nts::Tristate logicAnd(nts::Tristate a, nts::Tristate b);
nts::Tristate logicNand(nts::Tristate a, nts::Tristate b);
nts::Tristate logicNor(nts::Tristate a, nts::Tristate b);

class Or : public AComponent {
public:
  Or(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class And : public AComponent {
public:
  And(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class Xor : public AComponent {
public:
  Xor(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

class Not : public AComponent {
public:
  Not(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
} // namespace nts
