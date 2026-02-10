#pragma once

#include "Circuit.hpp"
#include <string>

namespace nts {
class Parser {
public:
  static void loadFile(const std::string &filename, Circuit &circuit);
};
} // namespace nts
