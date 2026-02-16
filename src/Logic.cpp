#include "Logic.hpp"
#include "IComponent.hpp"
#include <utility>

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
std::pair<nts::Tristate, nts::Tristate> simple_add(const nts::Tristate &lhs, const nts::Tristate &rhs, const nts::Tristate &cin) {
  nts::Tristate lhsxorrhs = !(lhs || rhs);
  nts::Tristate lhsandrhs = lhs && rhs;
  nts::Tristate res = !(lhsxorrhs || cin);
  nts::Tristate carry =  lhsandrhs || (lhsxorrhs && cin);
  return std::make_pair(res, carry);
}
