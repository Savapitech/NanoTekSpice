#pragma once

#include "AComponent.hpp"
#include "IComponent.hpp"
#include <cstddef>

namespace nts {
class C4008 : public AComponent {
public:
  C4008(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

//4013
class C4013 : public AComponent {
  private:
    class ffdata {
      private:
        nts::Tristate _data;
        nts::Tristate _lastClk;
      public:
        ffdata() : _data(nts::False), _lastClk(nts::False) {}
        nts::Tristate getData() const {
          return _data;
        }
        nts::Tristate getlastClk() const {
          return _lastClk;
        }
        void setData(nts::Tristate val) {
          _data = val;
        }
        void setlastClk(nts::Tristate val) {
          _lastClk = val;
        }
    };
    ffdata ff1;
    ffdata ff2;
  public:
    C4013(const std::string &name);
    nts::Tristate compute(std::size_t pin) override;
    nts::Tristate flip_flop(ffdata &ff, nts::Tristate clk, nts::Tristate reset, nts::Tristate data, nts::Tristate set); 
};

//4094

class C4094 : public AComponent {
  private:
    nts::Tristate _stage[8];
    nts::Tristate _output[8];
    nts::Tristate _lastClk;
  public:
    C4094(const std::string &name);
    nts::Tristate compute(std::size_t pin) override; 
};


class C4512 : public AComponent {
  public:
    C4512(const std::string &name);
    nts::Tristate compute(std::size_t pin) override;
};

class C4514 : public AComponent {
  public:
    C4514(const std::string &name);
    nts::Tristate compute(std::size_t pin) override;
};
}
