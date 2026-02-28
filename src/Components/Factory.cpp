#include <functional>
#include <map>
#include <memory>

#include "Components/Advanced.hpp"
#include "Components/Elementary.hpp"
#include "Components/Gates.hpp"
#include "Factory.hpp"
#include "IComponent.hpp"

using Creator = std::function<std::unique_ptr<nts::IComponent>(const std::string &)>;

static const std::map<std::string, Creator> factories = {
  // Special
  {"input",  [](const std::string &n) { return std::make_unique<nts::InputComponent>(n); }},
  {"output", [](const std::string &n) { return std::make_unique<nts::OutputComponent>(n); }},
  {"true",   [](const std::string &n) { return std::make_unique<nts::TrueComponent>(n); }},
  {"false",  [](const std::string &n) { return std::make_unique<nts::FalseComponent>(n); }},
  {"clock",  [](const std::string &n) { return std::make_unique<nts::ClockComponent>(n); }},
  {"logger", [](const std::string &n) { return std::make_unique<nts::LoggerComponent>(n); }},
  // Elementary
  {"and",    [](const std::string &n) { return std::make_unique<nts::AndComponent>(n); }},
  {"or",     [](const std::string &n) { return std::make_unique<nts::OrComponent>(n); }},
  {"xor",    [](const std::string &n) { return std::make_unique<nts::XorComponent>(n); }},
  {"not",    [](const std::string &n) { return std::make_unique<nts::NotComponent>(n); }},
  // Gates
  {"4081",   [](const std::string &n) { return std::make_unique<nts::C4081>(n); }},
  {"4001",   [](const std::string &n) { return std::make_unique<nts::C4001>(n); }},
  {"4011",   [](const std::string &n) { return std::make_unique<nts::C4011>(n); }},
  {"4030",   [](const std::string &n) { return std::make_unique<nts::C4030>(n); }},
  {"4071",   [](const std::string &n) { return std::make_unique<nts::C4071>(n); }},
  {"4069",   [](const std::string &n) { return std::make_unique<nts::C4069>(n); }},
  // Advanced
  {"4008",   [](const std::string &n) { return std::make_unique<nts::C4008>(n); }},
  {"4013",   [](const std::string &n) { return std::make_unique<nts::C4013>(n); }},
  {"4017",   [](const std::string &n) { return std::make_unique<nts::C4017>(n); }},
  {"4040",   [](const std::string &n) { return std::make_unique<nts::C4040>(n); }},
  {"4094",   [](const std::string &n) { return std::make_unique<nts::C4094>(n); }},
  {"4512",   [](const std::string &n) { return std::make_unique<nts::C4512>(n); }},
  {"4514",   [](const std::string &n) { return std::make_unique<nts::C4514>(n); }},
  {"4801",   [](const std::string &n) { return std::make_unique<nts::C4801>(n); }},
  {"2716",   [](const std::string &n) { return std::make_unique<nts::C2716>(n); }},
};

namespace nts {
std::unique_ptr<nts::IComponent> createComponent(const std::string &type,
                                                 const std::string name) {
  auto it = factories.find(type);
  if (it != factories.end())
    return it->second(name);
  throw std::runtime_error("Unknown component type: " + type);
}
} // namespace nts
