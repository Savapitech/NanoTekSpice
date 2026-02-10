#include <csignal>
#include <iostream>

#include "Circuit.hpp"
#include "Parser.hpp"

#define SUCCESSS 0;
#define FAILURE 84;

int main(int ac, char **av) {
  if (ac != 2) {
    std::cerr << "Usage: ./nanotekspice file.nts" << std::endl;
    return FAILURE;
  }

  try {
    nts::Circuit circuit;
    nts::Parser::loadFile(av[1], circuit);

    std::string command;
    std::cout << "> ";
    while (std::getline(std::cin, command)) {
      if (command == "exit")
        break;
      else if (command == "display")
        circuit.display();
      else if (command == "simulate")
        circuit.simulate();
      else if (command == "loop") {
        for (;;) {
          circuit.simulate();
          circuit.display();
        }
      } else {
        size_t eq = command.find('=');
        if (eq != std::string::npos) {
          std::string name = command.substr(0, eq);
          std::string val = command.substr(eq + 1);
          circuit.setInputValue(name, val);
        }
      }
      std::cout << "> ";
    }
    std::cout << "exit\n";
  } catch (const std::exception &e) {
    return std::cerr << e.what() << std::endl, FAILURE;
  }

  return SUCCESSS;
}
