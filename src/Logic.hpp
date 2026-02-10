#pragma once

#include "IComponent.hpp"

nts::Tristate operator&&(const nts::Tristate &lhs, const nts::Tristate &rhs);
nts::Tristate operator||(const nts::Tristate &lhs, const nts::Tristate &rhs);
nts::Tristate operator^(const nts::Tristate &lhs, const nts::Tristate &rhs);
nts::Tristate operator!(const nts::Tristate &rhs);
