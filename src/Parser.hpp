#pragma once

#include "Circuit.hpp"
#include <string>

#define ACLINK  _i++; if (_i > 10000) return nts::Undefined;
#define CLINK ACLINK

namespace nts {
class Parser {
public:
  static void loadFile(const std::string &filename, Circuit &circuit);
};
} // namespace nts
