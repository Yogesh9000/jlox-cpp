#pragma once

#include <iostream>
#include <string_view>

constexpr int EX_USAGE = 64;   // The command was used incorrectly
constexpr int EX_DATAERR = 65; // The input data was incorrect in some way
constexpr int EX_NOINPUT = 66; // An input file did not exist or was not readable.

namespace Error
{
  inline bool hadError = false;
}

inline void report(int line, std::string_view where, std::string_view message)
{
  std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
  Error::hadError = true;
}

inline void error(int line, std::string_view message)
{
  report(line, "", message);
}
