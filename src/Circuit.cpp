#include <algorithm>
#include <iostream>

#include "Circuit.hpp"
#include "Components/4081.hpp"
#include "Components/Elementary.hpp"
#include "Components/Gates.hpp"
#include "Components/Special.hpp"
#include "IComponent.hpp"

namespace nts {
Circuit::Circuit() : _tick(0) {}

void Circuit::addComponent(std::unique_ptr<nts::IComponent> component) {
  IComponent *ptr = component.get();

  if (ptr) {
    _componentByName[ptr->getName()] = ptr;

    if (dynamic_cast<InputComponent *>(ptr))
      _inputs.push_back(ptr);
    if (dynamic_cast<OutputComponent *>(ptr))
      _outputs.push_back(ptr);
  }
  _components.push_back(std::move(component));
}

nts::IComponent *Circuit::getComponent(const std::string &name) {
  if (_componentByName.find(name) != _componentByName.end())
    return _componentByName[name];
  return nullptr;
}

void Circuit::simulate() {
  _tick++;

  for (auto &comp : _components)
    comp->simulate(_tick);
}

void Circuit::display() {
  std::cout << "tick: " << _tick << std::endl;

  std::cout << "input(s):" << std::endl;
  std::sort(_inputs.begin(), _inputs.end(),
            [](nts::IComponent *a, nts::IComponent *b) {
              return dynamic_cast<IComponent *>(a)->getName() <
                     dynamic_cast<IComponent *>(b)->getName();
            });
  for (auto &in : _inputs) {
    IComponent *ac = dynamic_cast<IComponent *>(in);
    std::cout << "  " << ac->getName() << ": " << ac->compute(1) << std::endl;
  }

  std::cout << "output(s):" << std::endl;
  std::sort(_outputs.begin(), _outputs.end(),
            [](nts::IComponent *a, nts::IComponent *b) {
              return dynamic_cast<IComponent *>(a)->getName() <
                     dynamic_cast<IComponent *>(b)->getName();
            });
  for (auto &out : _outputs) {
    IComponent *ac = dynamic_cast<IComponent *>(out);
    std::cout << "  " << ac->getName() << ": " << ac->compute(1) << std::endl;
  }
}

void Circuit::setInputValue(const std::string &name, const std::string &value) {
  nts::IComponent *comp = getComponent(name);
  if (!comp)
    return;

  if (value == "1")
    comp->setValue(nts::True);
  else if (value == "0")
    comp->setValue(nts::False);
  else if (value == "U")
    comp->setValue(nts::Undefined);
}

std::unique_ptr<nts::IComponent>
Circuit::createComponent(const std::string &type, const std::string &name) {
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

  if (type == "and")
    return std::make_unique<And>(name);
  if (type == "or")
    return std::make_unique<Or>(name);
  if (type == "xor")
    return std::make_unique<Xor>(name);
  if (type == "not")
    return std::make_unique<Not>(name);

  if (type == "4081")
    return std::make_unique<C4081>(name);
  if (type == "4071")
    return std::make_unique<C4071>(name);
  if (type == "4030")
    return std::make_unique<C4030>(name);
  if (type == "4069")
    return std::make_unique<C4069>(name);
  if (type == "4001")
    return std::make_unique<C4001>(name);
  if (type == "4011")
    return std::make_unique<C4011>(name);

  throw std::runtime_error("Unknown component type: " + type);
}
} // namespace nts
