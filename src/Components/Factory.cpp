#include <functional>
#include <memory>
#include <utility>

#include "Factory.hpp"
#include "Components/Elementary.hpp"
#include "IComponent.hpp"

namespace nts {
  std::unique_ptr<nts::IComponent> createComponent(const std::string &type, const std::string &name) {
    std::map<std::string, std::function<std::unique_ptr<nts::IComponent>(void)>> components = {
      { "input", [name]{ return std::make_unique<InputComponent>(name); } },
      { "output", [name]{ return std::make_unique<OutputComponent>(name); } },
      { "true", [name]{ return std::make_unique<TrueComponent>(name); } },
      { "false", [name]{ return std::make_unique<FalseComponent>(name); } },
      { "clock", [name]{ return std::make_unique<ClockComponent>(name); } },
      { "and", [name]{ return std::make_unique<AndComponent>(name); } },
      { "or", [name]{ return std::make_unique<OrComponent>(name); } },
      { "xor", [name]{ return std::make_unique<XorComponent>(name); } },
      { "not", [name]{ return std::make_unique<NotComponent>(name); } }
    };

    auto comp = components.find(type);
    if (comp != components.end())
      return comp->second();
    throw std::runtime_error("Unknown component type: " + type);
  }
}
