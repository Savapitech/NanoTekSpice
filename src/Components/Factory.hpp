#pragma once

#include <memory>

#include "Elementary.hpp"
#include "IComponent.hpp"
#include "Special.hpp"

namespace nts {
  std::unique_ptr<IComponent> createComponent(const std::string &type, const std::string &name);
}
