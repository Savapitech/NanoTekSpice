#pragma once

#include <map>
#include <set>
#include <vector>

#include "IComponent.hpp"

namespace nts {
class AComponent : public IComponent {
private:
  const std::string _name;
  std::map<std::size_t, std::vector<std::pair<nts::IComponent *, std::size_t>>>
      _links;
  std::set<std::size_t> _computingPins;

protected:
  AComponent(const std::string &name);
  virtual ~AComponent() = default;
  nts::Tristate getPinValue(std::size_t pin);

public:
  virtual void simulate(std::size_t tick) override;
  virtual nts::Tristate compute(std::size_t pin) override;
  virtual void setLink(std::size_t pin, nts::IComponent &other,
                       std::size_t otherPin) override;

  virtual void setValue(nts::Tristate value) override;
  const std::string &getName() const override;
};
} // namespace nts
