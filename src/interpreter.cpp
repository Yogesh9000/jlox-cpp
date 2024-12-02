#include "interpreter.hpp"
#include "common.hpp"
#include "expr.hpp"
#include "lexer.hpp"

#include <any>
#include <iostream>


LoxRuntimeException::LoxRuntimeException(Token op, const std::string& msg)
  : op(std::move(op)), LoxException(msg)
{
}

bool Interpreter::is_truthy(const std::any &value)
{
  if (!value.has_value())
  {
    return false;
  }
  try {
    return std::any_cast<bool>(value);
  }
  catch (const std::bad_any_cast&)
  {
    return true;
  }
}

bool Interpreter::is_equal(const std::any &lhs, const std::any &rhs)
{
  if (!lhs.has_value() && !rhs.has_value())
  {
    return true;
  }
  if (!lhs.has_value())
  {
    return false;
  }
  if (lhs.type() != rhs.type())
  {
    return false;
  }
  if (lhs.type() == typeid(bool))
  {
    return std::any_cast<bool>(lhs) == std::any_cast<bool>(rhs);
  }
  if (lhs.type() == typeid(double))
  {
    return std::any_cast<double>(lhs) == std::any_cast<double>(rhs);
  }
  if (lhs.type() == typeid(std::string))
  {
    return std::any_cast<std::string>(lhs) == std::any_cast<std::string>(rhs);
  }

  // If the type is not bool, double, or std::string, return false
  return false;
}

std::string Interpreter::stringfy(const std::any &value)
{
  if (!value.has_value())
  {
    return "nil";
  }
  else if (value.type() == typeid(double))
  {
    return std::to_string(std::any_cast<double>(value));
  }
  else if (value.type() == typeid(bool))
  {
    if (std::any_cast<bool>(value))
    {
      return "true";
    }
    else {
      return "false";
    }
  }
  else if (value.type() == typeid(std::string))
  {
    return std::any_cast<std::string>(value);
  }
  return "";
}

template <typename... Args>
void Interpreter::check_number_operands(const Token &op, const std::any &value, Args... args)
{
  if (value.type() != typeid(double))
  {
    throw LoxRuntimeException(op, "Operands must be number");
  }
  if constexpr (sizeof...(args) > 0)
  {
    check_number_operands(op, args...);
  }
}

std::any Interpreter::evaluate(Expr &expr)
{
  return expr.accept(*this);
}

std::any Interpreter::visit_literal(Literal &expr)
{
  return expr.value.literal;
}

std::any Interpreter::visit_grouping(Grouping &expr)
{
  return evaluate(*(expr.expression));
}

std::any Interpreter::visit_unary(Unary &expr)
{
  auto right = evaluate(*(expr.right));
  switch (expr.op.type)
  {
    case TokenType::BANG : 
    {
      return !is_truthy(right);
    } break;
    case TokenType::MINUS :
    {
      check_number_operands(expr.op, right);
      return -(std::any_cast<double>(right));
    } break;
    default: break;
  }

  // unreachable
  return {};
}

std::any Interpreter::visit_binary(Binary &expr)
{
  auto left = evaluate(*(expr.left));
  auto right = evaluate(*(expr.right));

  switch (expr.op.type)
  {
    case TokenType::PLUS:
    {
      try {
        return std::any_cast<double>(left) + std::any_cast<double>(right);
      }
      catch (const std::bad_any_cast&) {
        try {
          return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
        }
        catch (const std::bad_any_cast& ex)
        {
          throw LoxRuntimeException(expr.op, "operator needs operands of type double or string");
        }
      }
    } break;
    case TokenType::MINUS: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    } break;
    case TokenType::STAR: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    } break;
    case TokenType::SLASH: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    } break;
    case TokenType::GREATER: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    } break;
    case TokenType::GREATER_EQUAL: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
    } break;
    case TokenType::LESS: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    } break;
    case TokenType::LESS_EQUAL: 
    {
      check_number_operands(expr.op , left, right);
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
    } break;
    case TokenType::BANG_EQUAL:
    {
      check_number_operands(expr.op , left, right);
      return !is_equal(left, right);
    }
    case TokenType::EQAUL_EQUAL:
    {
      check_number_operands(expr.op , left, right);
      return is_equal(left, right);
    }
    default: break;
  }

  // unreachable
  return {};
}


void Interpreter::interpret(Expr &expr)
{
  try {
    std::any value = evaluate(expr);
    std::cout << stringfy(value) << "\n";
  } catch (const LoxRuntimeException& ex )
  {
    std::cerr << "[line " << ex.op.line << "] " << ex.op.lexeme << " " << ex.what() << "\n";
    Error::hadRuntimeError = true;
  }
}
