#include "Logic.hpp"

nts::Tristate operator&&(const nts::Tristate &lhs, const nts::Tristate &rhs) {
  if (lhs == nts::False || rhs == nts::False)
    return nts::False;
  if (lhs == nts::True && rhs == nts::True)
    return nts::True;
  return nts::Undefined;
}

nts::Tristate operator||(const nts::Tristate &lhs, const nts::Tristate &rhs) {
  if (lhs == nts::True || rhs == nts::True)
    return nts::True;
  if (lhs == nts::False && rhs == nts::False)
    return nts::False;
  return nts::Undefined;
}

nts::Tristate operator^(const nts::Tristate &lhs, const nts::Tristate &rhs) {
  if (lhs == nts::Undefined || rhs == nts::Undefined)
    return nts::Undefined;
  return (lhs != rhs) ? nts::True : nts::False;
}

nts::Tristate operator!(const nts::Tristate &rhs) {
  if (rhs == nts::True)
    return nts::False;
  if (rhs == nts::False)
    return nts::True;
  return nts::Undefined;
}
