/*
** EPITECH PROJECT, 2025
** nanotekspice
** File description:
** CompassMain.cpp
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Compass.hpp"

static constexpr char const NTS_BIN_NAME[] = "./nanotekspice";

enum test_state_e {
  T_FORK_FAILURE = 1 << 0,
  T_IS_CHILD = 1 << 1,
  T_FAILURE = 1 << 2,
  T_SUCCESS = 1 << 3
};

static auto print_test_results(int status) -> enum test_state_e
{
  if (status & 128) {
    std::cerr
      << "\033[48;5;210m\033[38;5;232mCRASHED" RESET
         " with status\033[38;5;248m: " YELLOW
      << status << RESET "\n\n";
    return T_FAILURE;
  }
  std::cerr << "\n";
  return T_SUCCESS;
}

static auto run_test(char const *driver_file) -> enum test_state_e
{
  int status;
  pid_t pid = fork();

  if (pid < 0)
    return T_FORK_FAILURE;
  if (!pid) {
    run_autotest(NTS_BIN_NAME, driver_file);
    assert_debug("reach end of test", "", FILE_NAME, __LINE__, true);
    return T_IS_CHILD;
  }
  wait(&status);
  return print_test_results(status);
}

auto main(int argc, char **argv) -> int
{
  enum test_state_e status = T_SUCCESS;

  if (argc == 2) {
    std::cerr << "\033[38;5;103m╤══ \033[38;5;75m" << argv[1] << RESET ":\n";
    run_test(argv[1]);
    return EXIT_SUCCESS;
  }

  std::string name = "\033[38;5;189mCom\033[3mpass\033[23m\033[38;5;103m";
  std::cerr
    << "\033[38;5;103m"
    << "┌────────────────────────────┐\n"
    << "│          " << name << "           │\n"
    << "└────────────────────────────┘\n\n";

  DIR *fixture_dir = opendir("fixtures");
  if (fixture_dir == nullptr)
    return EXIT_FAILURE;

  for (struct dirent *entry = readdir(fixture_dir); entry != nullptr;
       entry = readdir(fixture_dir)) {
    std::string driver_filename("./fixtures/");
    driver_filename += entry->d_name;
    if (!driver_filename.ends_with(".autotest"))
      continue;

    std::cerr
      << "\033[38;5;103m╤══ \033[38;5;75m" << driver_filename << RESET ":\n";
    status = (enum test_state_e)(status | run_test(driver_filename.c_str()));
    if (status & T_IS_CHILD)
      break;
    if (status & T_FORK_FAILURE)
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void assert_debug(
  const std::string &out,
  const std::string &exp,
  const std::string &file,
  int line,
  bool terminal)
{
  if (out != exp || terminal) {
    std::cerr
      << "\033[38;5;103m" << (terminal ? "└── " : "├ ") << BLUE << "\033[4m"
      << file << RESET "\033[38;5;248m:" PURPLE << line << RESET << " " << out
      << RESET "\n";
  }
  if (out != exp && !terminal) {
    std::cerr
      << "\033[38;5;103m└ \033[38;5;1mX  Assertion failed!" RESET "\n"
      << "\033[3m\033[38;5;103m // expected: [" << exp << "] " << RESET "\n";
    usleep(500000);  // make the fail noticable!
    exit(EXIT_FAILURE);
  }
}
