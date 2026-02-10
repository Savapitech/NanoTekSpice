#include <memory>
// #include <functional>
// #include <utility>

#include "Components/Elementary.hpp"
#include "Components/Gates.hpp"
#include "Factory.hpp"
#include "IComponent.hpp"

namespace nts {
std::unique_ptr<nts::IComponent> createComponent(const std::string &type,
                                                 const std::string name) {
  // Specials
  if (type == "input")
    return std::make_unique<InputComponent>(name);
  if (type == "output")
    return std::make_unique<OutputComponent>(name);
  if (type == "true")
    return std::make_unique<TrueComponent>(name);
  if (type == "false")
    return std::make_unique<FalseComponent>(name);
  if (type == "clock")
    return std::make_unique<ClockComponent>(name);

  // Elementary
  if (type == "and")
    return std::make_unique<AndComponent>(name);
  if (type == "or")
    return std::make_unique<OrComponent>(name);
  if (type == "xor")
    return std::make_unique<XorComponent>(name);
  if (type == "not")
    return std::make_unique<NotComponent>(name);

  // Gates
  if (type == "4081")
    return std::make_unique<C4081>(name);
  throw std::runtime_error("Unknown component type: " + type);
}

// std::unique_ptr<nts::IComponent> createComponent(const std::string &type,
// const std::string name) {
//   std::map<std::string,
//   std::function<std::unique_ptr<nts::IComponent>(void)>> components = {
//     { "input", [name]{ return std::make_unique<InputComponent>(name); } },
//     { "output", [name]{ return std::make_unique<OutputComponent>(name); } },
//     { "true", [name]{ return std::make_unique<TrueComponent>(name); } },
//     { "false", [name]{ return std::make_unique<FalseComponent>(name); } },
//     { "clock", [name]{ return std::make_unique<ClockComponent>(name); } },
//     { "and", [name]{ return std::make_unique<AndComponent>(name); } },
//     { "or", [name]{ return std::make_unique<OrComponent>(name); } },
//     { "xor", [name]{ return std::make_unique<XorComponent>(name); } },
//     { "not", [name]{ return std::make_unique<NotComponent>(name); } }
//   };
//
//   auto comp = components.find(type);
//   if (comp != components.end())
//     return comp->second();
//   throw std::runtime_error("Unknown component type: " + type);
// }
} // namespace nts
