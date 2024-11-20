#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "print.hpp"


void run(const std::string &source)
{
  Scanner scanner{source};
  auto tokens = scanner.scan_tokens();
  Parser parser {tokens};
  auto expression = parser.parse();

  if (Error::hadError) return;
  AstPrinter printer;
  std::cout << printer.print(*expression) << "\n";
  expression = parser.parse();
}

void runFile(const std::string &fileName)
{
  std::ifstream fileHandle(fileName);
  if (!fileHandle.is_open())
  {
    std::exit(EX_NOINPUT);
  }
  std::stringstream ss;
  ss << fileHandle.rdbuf();
  std::string file = ss.str();
  run(file);
  if (Error::hadError)
  {
    std::exit(EX_DATAERR);
  }
}

void runPrompt()
{
  std::string input;
  while (true)
  {
    std::cout << "> ";
    std::cin >> input;
    if (!std::cin)
    {
      if (std::cin.eof())
      {
        break;
      }
      std::cin.clear();
    }
    run(input);
    if (Error::hadError)
    {
      Error::hadError = false;
    }
  }
}

int main(int argc, char **argv)
{
  if (argc > 2)
  {
    std::cerr << "Usage: jlox [script]" << "\n";
    std::exit(EX_USAGE);
  }
  else if (argc == 2)
  {
    runFile(argv[1]);
  }
  else
  {
    runPrompt();
  }

  return EXIT_SUCCESS;
}
