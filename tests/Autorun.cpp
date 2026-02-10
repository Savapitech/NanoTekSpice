/*
** EPITECH PROJECT, 2025
** nanotekspice
** File description:
** Autorun.cpp
*/

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "Compass.hpp"

static void check_line_match(FILE *procf, std::string &line)
{
  char *buff = nullptr;
  size_t sz;

  getline(&buff, &sz, procf);

  std::string out = std::string(buff);

  out = out.substr(0, out.length() - 1);
  while (out.starts_with("> "))
    out = out.substr(2);

  assert_str_match(out, line);
}

static auto
process_config_file(int in_fd, int out_fd, std::ifstream &file) -> std::string
{
  std::string config;
  FILE *procf = fdopen(out_fd, "r");

  while (std::getline(file, config)) {
    if (config.starts_with("#"))
      continue;
    if (config.starts_with("."))
      return config;
    if (config.starts_with("<- ")) {
      config = config.substr(3);
      std::cout << "\033[38;5;103m├ " RESET "sending [" << config << "]\n";
      write(in_fd, config.c_str(), config.size());
      write(in_fd, "\n", 1);
    }

    else {
      auto pos = config.find('#');
      if (pos != std::string::npos) {
        // we assume 1 space before the comment
        // because this garbage language as no tool to handle this properly
        config = config.substr(0, pos - 1);
      }

      check_line_match(procf, config);
    }
  }
  return "";
}

static auto run_autotest_process(
  char const *nts_prog,
  std::ifstream &driver_config,
  std::string &nts_circuit) -> std::string
{
  std::string ret;

  int in_pipe[2], out_pipe[2];  // NOLINT
  if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1) {
    std::cerr << "Failed to create pipes.\n";
    return "";
  }

  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "Fork failed.\n";
    return "";
  }

  if (pid == 0) {
    close(in_pipe[1]);
    close(out_pipe[0]);
    dup2(in_pipe[0], STDIN_FILENO);
    dup2(out_pipe[1], STDOUT_FILENO);
    close(in_pipe[0]);
    close(out_pipe[1]);

    execlp(nts_prog, nts_prog, nts_circuit.c_str(), nullptr);
    std::cerr << "Failed to execute process.\n";
    return "";
  }

  close(in_pipe[0]);
  close(out_pipe[1]);

  ret = process_config_file(in_pipe[1], out_pipe[0], driver_config);

  close(in_pipe[1]);
  close(out_pipe[0]);
  waitpid(pid, nullptr, 0);
  return ret;
}

auto run_autotest(char const *nts_process, char const *driver_filename) -> bool
{
  std::ifstream file(driver_filename);
  if (!file) {
    std::cerr << "Failed to open file.\n";
    return false;
  }

  std::string first_arg;
  if (!std::getline(file, first_arg)) {
    std::cerr << "Failed to read first argument from file.\n";
    return false;
  }

  while (first_arg != "") {
    std::cerr << "\033[38;5;103m├ " RESET "loading file:" << first_arg << "\n";

    if (!first_arg.starts_with("."))
      break;
    first_arg = run_autotest_process(nts_process, file, first_arg);
  }
  return true;
}
