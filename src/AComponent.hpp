#pragma once

#include <map>
#include <string>

#include "IComponent.hpp"

namespace nts {
class AComponent : public IComponent {
public:
  AComponent(const std::string &name);
  virtual ~AComponent() = default;

  virtual void simulate(std::size_t tick) override;
  virtual nts::Tristate compute(std::size_t pin) override;
  virtual void setLink(std::size_t pin, nts::IComponent &other,
                       std::size_t otherPin) override;

  virtual void setValue(nts::Tristate value) override { throw std::runtime_error("Cannot set value on a component that is not a input"); };

  const std::string &getName() const override;

protected:
  std::string _name;
  std::map<std::size_t, std::pair<nts::IComponent *, std::size_t>> _links;

  nts::Tristate getPinValue(std::size_t pin);
};
}
