#include "Advanced.hpp"
#include "AComponent.hpp"
#include "IComponent.hpp"
#include "Logic.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>

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
C4013::C4013(const std::string &name) : AComponent(name) {};

nts::Tristate C4013::flip_flop(ffdata &ff, nts::Tristate clk,
                               nts::Tristate reset, nts::Tristate data,
                               nts::Tristate set) {
  if (reset == nts::True) {
    ff.setData(nts::False);
    return nts::False;
  }
  if (set == nts::True) {
    ff.setData(nts::True);
    return nts::True;
  }
  if (ff.getlastClk() == nts::False && clk == nts::True) {
    ff.setData(data);
  }
  ff.setlastClk(clk);
  return ff.getData();
}

nts::Tristate C4013::compute(std::size_t pin) {
  nts::Tristate res;
  if (pin == 1 || pin == 2) {
    res = flip_flop(ff1, getPinValue(3), getPinValue(4), getPinValue(5),
                    getPinValue(6));
    return (pin == 1) ? res : !res;
  }
  if (pin == 13 || pin == 12) {
    res = flip_flop(ff2, getPinValue(11), getPinValue(10), getPinValue(9),
                    getPinValue(8));
    return (pin == 13) ? res : !res;
  }
  return nts::Undefined;
}

// C4017

C4017::C4017(const std::string &name) : AComponent(name) {
  _lastClk = nts::False;
  _val = 0;
}

nts::Tristate C4017::compute(std::size_t pin) {
  const std::size_t mapPins[10] = {3, 2, 4, 7, 10, 1, 5, 6, 9, 11};
  if (_lastClk == nts::True && getPinValue(14) == nts::False) {
    setVal((getVal() + 1) % 10);
  }
  if (getPinValue(15) == nts::True)
    setVal(0);

  setlastClk(getPinValue(14));

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

nts::Tristate C4040::compute(std::size_t pin) {
  const std::size_t mapPins[12] = {9, 7, 6, 5, 3, 2, 4, 13, 12, 14, 15, 1};
  if (getlastClk() == nts::True && getPinValue(10) == nts::False)
    setVal((getVal() + 1) % 4096);
  if (getPinValue(11) == nts::True)
    setVal(0);
  setlastClk(getPinValue(10));
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
nts::Tristate C4094::compute(std::size_t pin) {
  const std::size_t mapPins[8] = {4, 5, 6, 7, 14, 13, 12, 11};
  auto q_pin = std::find(std::begin(mapPins), std::end(mapPins), pin);

  if (_lastClk == nts::False && getPinValue(3) == nts::True) {
    _lastChange = _stage[7];
    for (int i = 7; i > 0; i--)
      _stage[i] = _stage[i - 1];
    _stage[0] = getPinValue(2);

    if (getPinValue(1) == nts::True) {
      for (int i = 0; i < 8; i++)
        _output[i] = _stage[i];
    }
  }

  _lastClk = getPinValue(3);

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
    if (getPinValue(10) == nts::True)
      return nts::False;
    if (getPinValue(15) == nts::True)
      return nts::Undefined;
    const std::size_t mapPins[8] = {1, 2, 3, 4, 5, 6, 7, 9};
    std::size_t out = to_bin(getPinValue(11)) + to_bin(getPinValue(12)) * 2 +
                      to_bin(getPinValue(13)) * 4;
    return getPinValue(mapPins[out]);
  }
  return nts::Undefined;
};

// 4514
C4514::C4514(const std::string &name) : AComponent(name) {};
nts::Tristate C4514::compute(std::size_t pin) {
  if (getPinValue(23) == nts::False)
    return nts::Undefined;
  std::size_t out = to_bin(getPinValue(2)) * 1 + to_bin(getPinValue(3)) * 2 +
                    to_bin(getPinValue(21)) * 4 + to_bin(getPinValue(22)) * 8;
  const std::size_t mapPins[16] = {11, 9,  10, 8,  7,  6,  5,  4,
                                   18, 17, 20, 19, 14, 13, 16, 15};
  if (pin == mapPins[out])
    return nts::True;
  else
    return nts::False;
};

C4801::C4801(const std::string &name) : AComponent(name) {};
nts::Tristate C4801::compute(std::size_t pin) {
  std::size_t mapPins[8] = {9, 10, 11, 13, 14, 15, 16, 17};
  std::size_t mapAddress[10] = {8, 7, 6, 5, 4, 3, 2, 1, 23, 22};
  std::size_t memadress = 0;
  for (int i = 0; i < 10; i++) {
    memadress += ((getPinValue(mapAddress[i]) == nts::True) ? 1 : 0) * (1 << i);
  }

  // write
  if (getPinValue(18) == nts::False && getPinValue(21) == nts::False) {
    std::uint8_t temp = 0;
    for (int i = 0; i < 8; i++) {
      if (getPinValue(mapPins[i]) == nts::True) {
        temp |= 1 << i;
      }
    }
    setmem(temp, memadress);
  }

  auto find = std::find(std::begin(mapPins), std::end(mapPins), pin);
  if (find == std::end(mapPins))
    return nts::Undefined;
  auto index = std::distance(std::begin(mapPins), find);

  // read
  if (getPinValue(18) == nts::False && getPinValue(20) == nts::False) {
    return ((getmem(memadress) >> index) & 1) ? nts::True : nts::False;
  }
  return nts::Undefined;
}

C2716::C2716(const std::string &name) : AComponent(name) {};
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
