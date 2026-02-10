/*
** EPITECH PROJECT, 2025
** nanotekspice
** File description:
** Compass.hpp
*/

#pragma once

#include <filesystem> /* IWYU pragma: export */
#include <string>
#include <sys/types.h>

#define COL_FMT(n) "\033[" #n "m"

#define RESET COL_FMT(0)
#define BOLD COL_FMT(1)

#define RED COL_FMT(31)
#define GREEN COL_FMT(32)
#define YELLOW COL_FMT(33)
#define BLUE COL_FMT(34)
#define PURPLE COL_FMT(35)
#define CYAN COL_FMT(36)

#define FILE_NAME (std::filesystem::path(__FILE__).filename().string())

using Callback = struct {
    char const *name;
    void (*func)();
};

auto run_autotest(char const *nts_process, char const *driver_filename)
  -> bool;

void assert_impl(bool res);

void assert_debug(
  const std::string &out,
  const std::string &exp,
  const std::string &file,
  int line,
  bool terminal = false);

#define assert_str_match(out, expected) \
    assert_debug(out, expected, FILE_NAME, __LINE__)
