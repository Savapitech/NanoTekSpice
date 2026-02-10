#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "Parser.hpp"

enum { STEP_CHIPSETS = 1 << 0, STEP_LINKS = 1 << 1 };

namespace nts {
void Parser::loadFile(const std::string &filename, Circuit &circuit) {
  std::ifstream file(filename);
  if (!file.is_open())
    throw std::runtime_error("Cannot open file");

  std::string line;
  uint8_t step = 0;

  while (std::getline(file, line)) {
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos)
      line = line.substr(0, commentPos);

    if (line.empty())
      continue;

    std::stringstream ss(line);
    std::string token;
    ss >> token;
    if (token.empty())
      continue;

    if (token == ".chipsets:") {
      step = STEP_CHIPSETS;
      continue;
    }
    if (token == ".links:") {
      step = STEP_LINKS;
      continue;
    }

    if (!step)
      throw std::runtime_error("Invalid label.");

    if (step & STEP_CHIPSETS) {
      std::string name;
      ss >> name;
      if (!name.empty()) {
        auto comp = Circuit::createComponent(token, name);
        circuit.addComponent(std::move(comp));
      }
    } else if (step & STEP_LINKS) {
      std::string p1 = token;
      std::string p2;
      ss >> p2;

      auto parsePin = [&](const std::string &s) {
        size_t colon = s.find(':');
        return std::make_pair(s.substr(0, colon),
                              std::stoi(s.substr(colon + 1)));
      };

      auto link1 = parsePin(p1);
      auto link2 = parsePin(p2);

      IComponent *c1 = circuit.getComponent(link1.first);
      IComponent *c2 = circuit.getComponent(link2.first);

      if (c1 && c2) {
        c1->setLink(link1.second, *c2, link2.second);
        c2->setLink(link2.second, *c1, link1.second);
      }
    }
  }
}
} // namespace nts
