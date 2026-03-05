#pragma once

#include "AComponent.hpp"
#include "Logic.hpp"

namespace nts {

struct AndOp {
  static Tristate apply(Tristate a, Tristate b) { return a && b; }
};
struct NandOp {
  static Tristate apply(Tristate a, Tristate b) { return !(a && b); }
};
struct OrOp {
  static Tristate apply(Tristate a, Tristate b) { return a || b; }
};
struct NorOp {
  static Tristate apply(Tristate a, Tristate b) { return !(a || b); }
};
struct XorOp {
  static Tristate apply(Tristate a, Tristate b) { return a ^ b; }
};

template <typename Op> class FourGateChip : public AComponent {
public:
  FourGateChip(const std::string &name) : AComponent(name) {}
  nts::Tristate compute(std::size_t pin) override {
    if (pin == 3)
      return Op::apply(getPinValue(1), getPinValue(2));
    if (pin == 4)
      return Op::apply(getPinValue(5), getPinValue(6));
    if (pin == 10)
      return Op::apply(getPinValue(8), getPinValue(9));
    if (pin == 11)
      return Op::apply(getPinValue(12), getPinValue(13));
    return nts::Undefined;
  }
};

using C4081 = FourGateChip<AndOp>;
using C4001 = FourGateChip<NorOp>;
using C4011 = FourGateChip<NandOp>;
using C4030 = FourGateChip<XorOp>;
using C4071 = FourGateChip<OrOp>;

// 4069 (six inverters — different pin layout)
class C4069 : public AComponent {
public:
  C4069(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};
} // namespace nts
