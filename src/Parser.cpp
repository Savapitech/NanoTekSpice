#include <cstdint>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>

#include "Components/Factory.hpp"
#include "Parser.hpp"

enum { STEP_CHIPSETS = 1 << 0, STEP_LINKS = 1 << 1 };

static std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r");
  if (start == std::string::npos)
    return "";
  size_t end = s.find_last_not_of(" \t\r");
  return s.substr(start, end - start + 1);
}

static std::pair<std::string, std::size_t> parsePin(const std::string &s,
                                                    size_t lineNb) {
  size_t colon = s.find(':');
  if (colon == std::string::npos || colon == 0 || colon == s.size() - 1)
    throw std::runtime_error("Line " + std::to_string(lineNb) +
                             ": Invalid link format `" + s + "'.");
  std::string name = s.substr(0, colon);
  std::string pinStr = s.substr(colon + 1);
  std::size_t pin = 0;
  try {
    size_t pos = 0;
    pin = std::stoul(pinStr, &pos);
    if (pos != pinStr.size())
      throw std::invalid_argument("trailing chars");
  } catch (...) {
    throw std::runtime_error("Line " + std::to_string(lineNb) +
                             ": Invalid pin number `" + pinStr + "'.");
  }
  if (pin == 0)
    throw std::runtime_error("Line " + std::to_string(lineNb) +
                             ": Pin number must be > 0.");
  return {name, pin};
}

namespace nts {
void Parser::loadFile(const std::string &filename, Circuit &circuit) {
  std::ifstream file(filename);
  if (!file.is_open())
    throw std::runtime_error("Cannot open file: " + filename);

  std::string line;
  uint8_t step = 0;
  size_t lineNb = 0;
  std::set<std::string> declaredNames;
  bool hasChipsets = false;

  while (std::getline(file, line)) {
    lineNb++;

    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos)
      line = line.substr(0, commentPos);

    line = trim(line);
    if (line.empty())
      continue;

    std::stringstream ss(line);
    std::string token;
    ss >> token;
    if (token.empty())
      continue;

    if (token == ".chipsets:") {
      std::string extra;
      if (ss >> extra)
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Unexpected token after `.chipsets:'.");
      step = STEP_CHIPSETS;
      continue;
    }
    if (token == ".links:") {
      std::string extra;
      if (ss >> extra)
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Unexpected token after `.links:'.");
      if (!hasChipsets)
        throw std::runtime_error("No chipsets in circuit.");
      step = STEP_LINKS;
      continue;
    }

    if (!step)
      throw std::runtime_error("Line " + std::to_string(lineNb) +
                               ": Content outside of a valid section.");

    if (step & STEP_CHIPSETS) {
      std::string type = token;
      std::string name;
      ss >> name;
      if (name.empty())
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Missing component name for type `" + type +
                                 "'.");
      std::string extra;
      if (ss >> extra)
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Unexpected token `" + extra + "'.");
      if (declaredNames.count(name))
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Duplicate component name `" + name + "'.");
      declaredNames.insert(name);
      auto comp = nts::createComponent(type, name);
      circuit.addComponent(std::move(comp));
      hasChipsets = true;
    } else if (step & STEP_LINKS) {
      std::string p1 = token;
      std::string p2;
      ss >> p2;
      if (p2.empty())
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Missing second link target.");
      std::string extra;
      if (ss >> extra)
        throw std::runtime_error("Line " + std::to_string(lineNb) +
                                 ": Unexpected token `" + extra + "'.");

      auto link1 = parsePin(p1, lineNb);
      auto link2 = parsePin(p2, lineNb);

      IComponent *c1 = circuit.getComponent(link1.first);
      IComponent *c2 = circuit.getComponent(link2.first);

      c1->setLink(link1.second, *c2, link2.second);
      c2->setLink(link2.second, *c1, link1.second);
    }
  }

  if (!hasChipsets)
    throw std::runtime_error("No chipsets in circuit.");
}
} // namespace nts
