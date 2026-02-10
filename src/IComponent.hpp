#pragma once

#include <cstddef>
#include <iostream>

namespace nts {
enum Tristate { Undefined = (-true), True = true, False = false };

class IComponent {
public:
  virtual ~IComponent() = default;

  virtual void simulate(std::size_t tick) = 0;
  virtual nts::Tristate compute(std::size_t pin) = 0;
  virtual void setLink(std::size_t pin, nts::IComponent &other,
                       std::size_t otherPin) = 0;

  virtual const std::string &getName() const = 0;
  virtual void setValue(nts::Tristate value) = 0;
};
} // namespace nts

std::ostream &operator<<(std::ostream &s, nts::Tristate v);
