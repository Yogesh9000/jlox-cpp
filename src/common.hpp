#pragma once

#include <iostream>
#include <string_view>

// The command was used incorrectly
constexpr int EX_USAGE = 64;
// The input data was incorrect in some way
constexpr int EX_DATAERR = 65;
// An input file did not exist or was not readable.
constexpr int EX_NOINPUT = 66;
// Runtime Error
constexpr int EX_RUNTIME = 70;

namespace Error
{
  // hadError is set whenever there is any error during compilation process
  inline bool hadError = false;
  inline bool hadRuntimeError = false;
}

inline void report(int line, std::string_view where, std::string_view message)
{
  std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
  Error::hadError = true;
}

// writes a error message to stderr
inline void error(int line, std::string_view message)
{
  report(line, "", message);
}

struct LoxException : public std::runtime_error
{
  explicit LoxException(const std::string& what = "")
    : std::runtime_error(what)
  {
  }
};
