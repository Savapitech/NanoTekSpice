#include "Advanced.hpp"
#include "AComponent.hpp"
#include "Error.hpp"
#include "IComponent.hpp"
#include "Logic.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

// 4008
namespace nts {
C4008::C4008(const std::string &name) : AComponent(name) {};
nts::Tristate C4008::compute(std::size_t pin) {
  if (pin == 10)
    return simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).first;
  if (pin == 11) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    return simple_add(getPinValue(4), getPinValue(5), c1).first;
  }
  if (pin == 12) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    return simple_add(getPinValue(2), getPinValue(3), c2).first;
  }
  if (pin == 13) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    nts::Tristate c3 = simple_add(getPinValue(2), getPinValue(3), c2).second;
    return simple_add(getPinValue(15), getPinValue(1), c3).first;
  }
  if (pin == 14) {
    nts::Tristate c1 =
        simple_add(getPinValue(6), getPinValue(7), getPinValue(9)).second;
    nts::Tristate c2 = simple_add(getPinValue(4), getPinValue(5), c1).second;
    nts::Tristate c3 = simple_add(getPinValue(2), getPinValue(3), c2).second;
    nts::Tristate c4 = simple_add(getPinValue(15), getPinValue(1), c3).second;
    return c4;
  }

  return nts::Undefined;
};

std::size_t to_bin(nts::Tristate state) {
  if (state == nts::True)
    return 1;
  else {
    return 0;
  }
}

// 4013
C4013::C4013(const std::string &name) : AComponent(name) {}

void C4013::simulate(const std::size_t tick) {
  (void)tick;
  auto clk1 = getPinValue(3), rst1 = getPinValue(4), dat1 = getPinValue(5),
       set1 = getPinValue(6);
  auto clk2 = getPinValue(11), rst2 = getPinValue(10), dat2 = getPinValue(9),
       set2 = getPinValue(8);
  flip_flop(ff1, clk1, rst1, dat1, set1);
  flip_flop(ff2, clk2, rst2, dat2, set2);
}

void C4013::flip_flop(ffdata &ff, nts::Tristate clk, nts::Tristate reset,
                      nts::Tristate data, nts::Tristate set) {
  if (reset == nts::True) {
    ff.setData(nts::False);
    ff.setlastClk(clk);
    return;
  }
  if (set == nts::True) {
    ff.setData(nts::True);
    ff.setlastClk(clk);
    return;
  }
  if (ff.getlastClk() == nts::False && clk == nts::True) {
    ff.setData(data);
  }
  ff.setlastClk(clk);
}

nts::Tristate C4013::compute(std::size_t pin) {
  if (pin == 1)
    return ff1.getData();
  if (pin == 2)
    return !ff1.getData();
  if (pin == 13)
    return ff2.getData();
  if (pin == 12)
    return !ff2.getData();
  return nts::Undefined;
}

// C4017

C4017::C4017(const std::string &name) : AComponent(name) {
  _lastClk = nts::False;
  _val = 0;
}

void C4017::simulate(std::size_t tick) {
  (void)tick;
  nts::Tristate cp0 = getPinValue(14);
  nts::Tristate cp1 = getPinValue(13);
  nts::Tristate effectiveClk = cp0 && !cp1;

  if (_lastClk == nts::False && effectiveClk == nts::True)
    setVal((getVal() + 1) % 10);
  if (getPinValue(15) == nts::True)
    setVal(0);
  setlastClk(effectiveClk);
}

nts::Tristate C4017::compute(std::size_t pin) {
  const std::size_t mapPins[10] = {3, 2, 4, 7, 10, 1, 5, 6, 9, 11};

  if (pin == 12)
    return (getVal() < 5) ? nts::True : nts::False;

  auto find = std::find(std::begin(mapPins), std::end(mapPins), pin);
  if (find == std::end(mapPins))
    return nts::Undefined;
  auto index = std::distance(std::begin(mapPins), find);
  return (getVal() == static_cast<std::size_t>(index)) ? nts::True : nts::False;
}

// C4040

C4040::C4040(const std::string &name) : AComponent(name) {
  _lastClk = nts::False;
  _val = 0;
}

void C4040::simulate(std::size_t tick) {
  (void)tick;
  nts::Tristate clk = getPinValue(10);
  if (getlastClk() == nts::True && clk == nts::False)
    setVal((getVal() + 1) % 4096);
  if (getPinValue(11) == nts::True)
    setVal(0);
  setlastClk(clk);
}

nts::Tristate C4040::compute(std::size_t pin) {
  const std::size_t mapPins[12] = {9, 7, 6, 5, 3, 2, 4, 13, 12, 14, 15, 1};
  auto find = std::find(std::begin(mapPins), std::end(mapPins), pin);
  if (find == std::end(mapPins))
    return nts::Undefined;
  auto index = std::distance(std::begin(mapPins), find);
  return ((getVal() >> index) & 1) ? nts::True : nts::False;
}

// C4094

C4094::C4094(const std::string &name) : AComponent(name) {
  _lastClk = nts::False;
  _lastChange = nts::False;
  for (int i = 0; i < 8; i++) {
    _output[i] = nts::False;
    _stage[i] = nts::False;
  }
};

void C4094::simulate(std::size_t tick) {
  (void)tick;
  nts::Tristate clk = getPinValue(3);
  if (_lastClk == nts::False && clk == nts::True) {
    _lastChange = _stage[7];
    for (int i = 7; i > 0; i--)
      _stage[i] = _stage[i - 1];
    _stage[0] = getPinValue(2);
    if (getPinValue(1) == nts::True) {
      for (int i = 0; i < 8; i++)
        _output[i] = _stage[i];
    }
  }
  _lastClk = clk;
}

nts::Tristate C4094::compute(std::size_t pin) {
  const std::size_t mapPins[8] = {4, 5, 6, 7, 14, 13, 12, 11};
  auto q_pin = std::find(std::begin(mapPins), std::end(mapPins), pin);

  if (q_pin != std::end(mapPins)) {
    std::size_t index = std::distance(std::begin(mapPins), q_pin);
    if (getPinValue(15) == nts::True)
      return _output[index];
    else
      return nts::Undefined;
  }

  if (pin == 9)
    return _stage[7];
  if (pin == 10)
    return _lastChange;

  return nts::Undefined;
};

// 4512
C4512::C4512(const std::string &name) : AComponent(name) {};
nts::Tristate C4512::compute(std::size_t pin) {
  if (pin == 14) {
    if (getPinValue(15) == nts::True)
      return nts::Undefined;

    if (getPinValue(10) == nts::True)
      return nts::False;

    if (getPinValue(11) == nts::Undefined ||
        getPinValue(12) == nts::Undefined || getPinValue(13) == nts::Undefined)
      return nts::Undefined;

    const std::size_t mapPins[8] = {1, 2, 3, 4, 5, 6, 7, 9};
    std::size_t out = 0;
    if (getPinValue(11) == nts::True)
      out += 1;
    if (getPinValue(12) == nts::True)
      out += 2;
    if (getPinValue(13) == nts::True)
      out += 4;

    return getPinValue(mapPins[out]);
  }
  return nts::Undefined;
};

// 4514
C4514::C4514(const std::string &name)
    : AComponent(name), _lastStrobe(nts::Undefined), _valA(nts::Undefined),
      _valB(nts::Undefined), _valC(nts::Undefined), _valD(nts::Undefined) {};

void C4514::simulate(std::size_t tick) {
  (void)tick;
  nts::Tristate strobe = getPinValue(1);
  if (getlastStrobe() == nts::True && strobe == nts::False) {
    _valA = getPinValue(2);
    _valB = getPinValue(3);
    _valC = getPinValue(21);
    _valD = getPinValue(22);
  }
  setlastStrobe(strobe);
}

nts::Tristate C4514::compute(std::size_t pin) {
  if (getPinValue(23) == nts::True)
    return nts::False;

  nts::Tristate strobe = getPinValue(1);
  if (strobe == nts::True)
    return nts::False;

  if (_valA == nts::Undefined && _valB == nts::Undefined &&
      _valC == nts::Undefined && _valD == nts::Undefined)
    return nts::Undefined;

  std::size_t out = to_bin(_valA) * 1 + to_bin(_valB) * 2 + to_bin(_valC) * 4 +
                    to_bin(_valD) * 8;
  const std::size_t mapPins[16] = {11, 9,  10, 8,  7,  6,  5,  4,
                                   18, 17, 20, 19, 14, 13, 16, 15};
  if (pin == mapPins[out])
    return nts::True;
  else
    return nts::False;
};

C4801::C4801(const std::string &name) : AComponent(name) {};

void C4801::simulate(std::size_t tick) {
  (void)tick;
  if (getPinValue(18) == nts::False && getPinValue(21) == nts::False) {
    std::size_t mapPins[8] = {9, 10, 11, 13, 14, 15, 16, 17};
    std::size_t mapAddress[10] = {8, 7, 6, 5, 4, 3, 2, 1, 23, 22};
    std::size_t memadress = 0;
    for (int i = 0; i < 10; i++) {
      memadress +=
          ((getPinValue(mapAddress[i]) == nts::True) ? 1 : 0) * (1 << i);
    }
    std::uint8_t temp = 0;
    for (int i = 0; i < 8; i++) {
      if (getPinValue(mapPins[i]) == nts::True) {
        temp |= (1 << i);
      }
    }
    setmem(temp, memadress);
  }
}

nts::Tristate C4801::compute(std::size_t pin) {
  std::size_t mapPins[8] = {9, 10, 11, 13, 14, 15, 16, 17};
  std::size_t mapAddress[10] = {8, 7, 6, 5, 4, 3, 2, 1, 23, 22};
  std::size_t memadress = 0;

  for (int i = 0; i < 10; i++) {
    memadress += ((getPinValue(mapAddress[i]) == nts::True) ? 1 : 0) * (1 << i);
  }

  auto find = std::find(std::begin(mapPins), std::end(mapPins), pin);
  if (find == std::end(mapPins))
    return nts::Undefined;
  auto index = std::distance(std::begin(mapPins), find);

  if (getPinValue(18) == nts::False && getPinValue(20) == nts::False) {
    return ((getmem(memadress) >> index) & 1) ? nts::True : nts::False;
  }
  return nts::Undefined;
}

C2716::C2716(const std::string &name) : AComponent(name) {
  std::ifstream file("./rom.bin", std::ios::binary);

  if (!file.is_open())
    throw nts::FileError("no rom.bin to read");
  file.read((char *)_mem.data(), _mem.size());
};

nts::Tristate C2716::compute(std::size_t pin) {
  std::size_t mapPins[8] = {9, 10, 11, 13, 14, 15, 16, 17};
  std::size_t mapAddress[11] = {8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19};
  std::size_t memadress = 0;
  for (int i = 0; i < 11; i++) {
    memadress += ((getPinValue(mapAddress[i]) == nts::True) ? 1 : 0) * (1 << i);
  }

  auto find = std::find(std::begin(mapPins), std::end(mapPins), pin);
  if (find == std::end(mapPins))
    return nts::Undefined;
  auto index = std::distance(std::begin(mapPins), find);

  if (getPinValue(18) == nts::False && getPinValue(20) == nts::False) {
    return ((getmem(memadress) >> index) & 1) ? nts::True : nts::False;
  }
  return nts::Undefined;
}

} // namespace nts
