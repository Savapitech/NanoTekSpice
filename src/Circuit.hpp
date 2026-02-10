#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "IComponent.hpp"

namespace nts {
class Circuit {
public:
  Circuit();
  ~Circuit() = default;

  void addComponent(std::unique_ptr<nts::IComponent> component);
  nts::IComponent *getComponent(const std::string &name);

  void simulate();
  void display();
  void setInputValue(const std::string &name, const std::string &value);

  static std::unique_ptr<nts::IComponent>
  createComponent(const std::string &type, const std::string &name);

private:
  std::vector<std::unique_ptr<nts::IComponent>> _components;
  std::map<std::string, nts::IComponent *> _componentByName;
  std::size_t _tick;

  std::vector<nts::IComponent *> _inputs;
  std::vector<nts::IComponent *> _outputs;
};
}
