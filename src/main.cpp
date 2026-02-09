#include <csignal>
#include <iostream>


#define SUCCESSS 0;
#define FAILURE 84;

int main(int ac, char **av) {
  if (ac != 2) {
    std::cerr << "Usage: ./nanotekspice file.nts" << std::endl;
    return FAILURE;
  }

  return SUCCESSS;
}
