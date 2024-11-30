#pragma once

#include "expr.hpp"
#include "lexer.hpp"
#include <any>
#include <format>
#include <memory>
#include <string>

struct AstPrinter : public ExprVisitor
{
  std::string print(Expr &expr)
  {
    return std::any_cast<std::string>(expr.accept(*this));
  }

  std::any visit_binary(Binary &expr) override
  {
    return parenthesize(expr.op.lexeme, {*(expr.left), *(expr.right)});
  }
  std::any visit_grouping(Grouping &expr) override
  {
    return parenthesize("group", {*(expr.expression)});
  }
  std::any visit_literal(Literal &expr) override
  {
    if (expr.value.type == TokenType::STRING || expr.value.type == TokenType::NUMBER)
    {
      return expr.value.lexeme;
    }
    return std::string("nil");
  }
  std::any visit_unary(Unary &expr) override
  {
    return parenthesize(expr.op.lexeme, {*(expr.right)});
  }

  std::string parenthesize(const std::string &name, const std::vector<std::reference_wrapper<Expr>>& arg)
  {
    std::string str = std::format("({}", name);
    for(const auto &expr : arg)
    {
      auto ss = expr.get().accept(*this);
      str += std::format(" {}", std::any_cast<std::string>(ss));
    }
    str += ")";
    return str;
  }
};
