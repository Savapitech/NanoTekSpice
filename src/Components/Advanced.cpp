#include "AComponent.hpp"
#include "IComponent.hpp"
#include "Advanced.hpp"
#include "Logic.hpp"
#include <cstddef>

// 4008

namespace nts {
C4008::C4008(const std::string &name) : AComponent(name) {};
nts::Tristate C4008::compute(std::size_t pin) {
  if (pin == 10)
    return simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).first;
  if (pin == 11) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    return simple_add(getPinValue(4), getPinValue(5), c1).first;
  }
  if (pin == 12) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    return simple_add(getPinValue(2), getPinValue(3), c2).first;
  }
  if (pin == 13) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    nts::Tristate c3 = simple_add(getPinValue(2), getPinValue(3), c2).second;
    return simple_add(getPinValue(15), getPinValue(1), c3).first;
  }
  if (pin == 14) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    nts::Tristate c3 = simple_add(getPinValue(2), getPinValue(3), c2).second;
    nts::Tristate c4 = simple_add(getPinValue(15), getPinValue(1), c3).second;
    return c4;
  }

  return nts::Undefined;
};

std::size_t to_bin(nts::Tristate state) {
  if (state == nts::True)
    return 1;
  else {
    return 0;
  }
}

//4514
C4514::C4514(const std::string &name) : AComponent(name) {};
nts::Tristate C4514::compute(std::size_t pin) {
  if (getPinValue(23) == nts::True)
    return nts::False;
  std::size_t out = to_bin(getPinValue(2)) * 1 + to_bin(getPinValue(3)) * 2 + to_bin(getPinValue(21)) * 4 + to_bin(getPinValue(22)) * 8;
  const std::size_t mapPins[16] = {
    11, 9, 10, 8, 7, 6, 5, 4,18, 17, 20, 19, 14, 13, 16, 15
  };
  if (pin == mapPins[out])
    return nts::True;
  else return nts::False;
};
}
