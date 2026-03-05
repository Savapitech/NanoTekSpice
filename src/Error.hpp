#pragma once
#include <stdexcept>
#include <string>

namespace nts {

class NtsError : public std::exception {
private:
  std::string _msg;

public:
  NtsError(const std::string &msg) : _msg(msg) {}
  const char *what() const noexcept override { return _msg.c_str(); }
};

class ParseError : public NtsError {
public:
  ParseError(const std::string &msg) : NtsError(msg) {}
};

class UnknownComponentError : public NtsError {
public:
  UnknownComponentError(const std::string &msg) : NtsError(msg) {}
};

class DuplicateComponentError : public NtsError {
public:
  DuplicateComponentError(const std::string &msg) : NtsError(msg) {}
};

class PinError : public NtsError {
public:
  PinError(const std::string &msg) : NtsError(msg) {}
};

class FileError : public NtsError {
public:
  FileError(const std::string &msg) : NtsError(msg) {}
};

class CircuitError : public NtsError {
public:
  CircuitError(const std::string &msg) : NtsError(msg) {}
};

class ComponentError : public NtsError {
public:
  ComponentError(const std::string &msg) : NtsError(msg) {}
};

} // namespace nts
