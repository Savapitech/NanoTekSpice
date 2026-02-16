#pragma once

#include "IComponent.hpp"

nts::Tristate operator&&(const nts::Tristate &lhs, const nts::Tristate &rhs);
nts::Tristate operator||(const nts::Tristate &lhs, const nts::Tristate &rhs);
nts::Tristate operator^(const nts::Tristate &lhs, const nts::Tristate &rhs);
nts::Tristate operator!(const nts::Tristate &rhs);
std::pair<nts::Tristate, nts::Tristate> simple_add(const nts::Tristate &lhs,
                                                   const nts::Tristate &rhs,
                                                   const nts::Tristate &cin);
