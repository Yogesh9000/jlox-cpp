#pragma once

#include "common.hpp"
#include "expr.hpp"
#include "lexer.hpp"
#include <any>
#include <utility>

struct LoxRuntimeException : public LoxException
{
public:
  LoxRuntimeException(Token op, const std::string& msg);

  Token op;
};


class Interpreter : public ExprVisitor
{
public:
  void interpret(Expr &expr);
  std::any visit_binary(Binary &expr) override;
  std::any visit_grouping(Grouping &expr) override;
  std::any visit_literal(Literal &expr) override;
  std::any visit_unary(Unary &expr) override;

private:
  std::any evaluate(Expr &expr);
  static bool is_truthy(const std::any &value);
  static bool is_equal(const std::any &lhs, const std::any &rhs);
  static std::string stringfy(const std::any &value);

  template <typename... Args>
  void check_number_operands(const Token &op, const std::any &value, Args... args);
};
