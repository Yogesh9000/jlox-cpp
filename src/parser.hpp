#pragma once

#include "common.hpp"
#include "lexer.hpp"
#include "expr.hpp"
#include <memory>

struct ParserException : LoxException
{
  explicit ParserException(const std::string &what) : LoxException(what)
  {
  }
};

class Parser
{
public:
  using ExprNode = std::unique_ptr<Expr>;

public:
  explicit Parser(std::vector<Token> tokens);

  ExprNode parse();

  ExprNode expression();
  ExprNode equality();
  ExprNode comparison();
  ExprNode term();
  ExprNode factor();
  ExprNode unary();
  ExprNode primary();
  bool is_at_end();

private:
  Token& peek();
  Token& previous();
  Token& advance();
  bool check(const TokenType &type);
  Token& consume(const TokenType &type, const std::string &message);
  template<typename... Args>
  bool match(const TokenType &type, Args... rest);
  void synchronize();

  static ParserException error(const Token &token, const std::string &message);

  std::vector<Token> m_tokens;
  int m_current;
};
