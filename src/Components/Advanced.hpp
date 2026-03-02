#pragma once

#include "AComponent.hpp"
#include "IComponent.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

namespace nts {
class C4008 : public AComponent {
public:
  C4008(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
};

// 4013
class C4013 : public AComponent {
private:
  class ffdata {
  private:
    nts::Tristate _data;
    nts::Tristate _lastClk;

  public:
    ffdata() : _data(nts::False), _lastClk(nts::False) {}
    nts::Tristate getData() const { return _data; }
    nts::Tristate getlastClk() const { return _lastClk; }
    void setData(nts::Tristate val) { _data = val; }
    void setlastClk(nts::Tristate val) { _lastClk = val; }
  };
  ffdata ff1;
  ffdata ff2;
  nts::Tristate done_1;
  nts::Tristate done_2;

public:
  C4013(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  void simulate(std::size_t pin) override;
  nts::Tristate flip_flop(ffdata &ff, nts::Tristate clk, nts::Tristate reset,
                          nts::Tristate data, nts::Tristate set);
};

// 4017

class C4017 : public AComponent {
private:
  nts::Tristate _lastClk;
  std::size_t _val;

public:
  C4017(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  std::size_t getVal() const { return _val; }
  nts::Tristate getlastClk() const { return _lastClk; }
  void setVal(std::size_t val) { _val = val; }
  void setlastClk(nts::Tristate val) { _lastClk = val; }
};

// 4040
class C4040 : public AComponent {
private:
  nts::Tristate _lastClk;
  std::size_t _val;

public:
  C4040(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  std::size_t getVal() const { return _val; }
  nts::Tristate getlastClk() const { return _lastClk; }
  void setVal(std::size_t val) { _val = val; }
  void setlastClk(nts::Tristate val) { _lastClk = val; }
};

// 4094
class C4094 : public AComponent {
private:
  nts::Tristate _stage[8];
  nts::Tristate _output[8];
  nts::Tristate _lastClk;
  nts::Tristate _lastChange;

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
private:
  nts::Tristate _lastStrobe;
  nts::Tristate _valA;
  nts::Tristate _valB;
  nts::Tristate _valC;
  nts::Tristate _valD;

public:
  C4514(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  nts::Tristate getlastStrobe() const { return _lastStrobe; }
  void setlastStrobe(nts::Tristate val) { _lastStrobe = val; }
};

class C2716 : public AComponent {
private:
  std::array<uint8_t, 2048> _mem{};

public:
  C2716(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  std::uint8_t getmem(std::size_t i) const { return _mem[i]; }
  void setmem(std::uint8_t val, std::size_t i) { _mem[i] = val; }
};

class C4801 : public AComponent {
private:
  std::uint8_t _mem[1024] = {};

public:
  C4801(const std::string &name);
  nts::Tristate compute(std::size_t pin) override;
  std::uint8_t getmem(std::size_t i) const { return _mem[i]; }
  void setmem(std::uint8_t val, std::size_t i) { _mem[i] = val; }
};
} // namespace nts
