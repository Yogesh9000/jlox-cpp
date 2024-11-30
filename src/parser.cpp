#include "parser.hpp"

#include <memory>
#include <utility>
#include "expr.hpp"
#include "lexer.hpp"

using ExprNode = Parser::ExprNode;

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_current(0)
{
}

Token& Parser::peek()
{
  return m_tokens[m_current];
}

Token& Parser::previous()
{
  return m_tokens[m_current - 1];
}

Token& Parser::advance()
{
  if (!is_at_end())
  {
    ++m_current;
  }
  return previous();
}

bool Parser::is_at_end()
{
  return peek().type == TokenType::eof;
}

bool Parser::check(const TokenType &type)
{
  if (peek().type == TokenType::eof)
  {
    return false;
  }
  return peek().type == type;
}

Token& Parser::consume(const TokenType &type, const std::string &message)
{
  if (check(type)) return advance();
  throw error(peek(), message);
}

ParserException Parser::error(const Token &token, const std::string &message)
{
  if (token.type == TokenType::eof) {
    report(token.line, " at end", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }
  return ParserException(message);
}

void Parser::synchronize()
{
  advance();
  while (peek().type != TokenType::eof)
  {
    // TODO: Need to understand why previous and not peek
    if (previous().type == TokenType::SEMICOLON) return;

    switch(peek().type)
    {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
        return;
      default:
        break;
    }
    advance();
  }
}

template<typename... Args>
bool Parser::match(const TokenType &type, Args... rest)
{
  if (check(type))
  {
    advance();
    return true;
  }
  else if constexpr (sizeof...(rest) > 0)
  {
    return match(rest...); 
  }
  return false;
}


ExprNode Parser::expression()
{
  return equality();
}

ExprNode Parser::equality()
{
  auto expr = comparison();
  while (match(TokenType::BANG_EQUAL, TokenType::EQAUL_EQUAL))
  {
    Token op = previous();
    ExprNode right = comparison();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprNode Parser::comparison()
{
  auto expr = term();
  while (match(TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL))
  {
    auto op = previous();
    auto right = term();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprNode Parser::term()
{
  auto expr = factor();
  while (match(TokenType::MINUS, TokenType::PLUS))
  {
    auto op = previous();
    auto right = factor();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprNode Parser::factor()
{
  auto expr = unary();
  while (match(TokenType::SLASH, TokenType::STAR))
  {
    auto op = previous();
    auto right = unary();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprNode Parser::unary()
{
  while (match(TokenType::BANG, TokenType::MINUS))
  {
    auto op = previous();
    auto right = unary();
    return std::make_unique<Unary>(op, std::move(right));
  }
  return primary();
}

ExprNode Parser::primary()
{
  if (match(TokenType::FALSE, TokenType::TRUE, TokenType::NIL, TokenType::NUMBER, TokenType::STRING))
  {
    return std::make_unique<Literal>(previous());
  }
  if (match(TokenType::LEFT_PAREN))
  {
    ExprNode expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expected closing paranthesis");
    return std::make_unique<Grouping>(std::move(expr));
  }
  throw error(peek(), "Expected expression");
}

ExprNode Parser::parse()
{ 
  try {
    return expression();
  } catch (const ParserException& parse_exception) {
    std::cout << parse_exception.what() << "\n";
  }
  return nullptr;
}
