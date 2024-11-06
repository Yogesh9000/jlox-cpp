#include "common.hpp"
#include <cctype>
#include "lexer.hpp"

std::string to_string(TokenType type)
{
  switch (type)
  {

    TokenFunc(GenEnumStr)
    default: return "Unknown Token Type"; break;
  }
}

std::string Token::to_string() const
{
  return ::to_string(type) + " \"" + lexeme + "\" " + std::to_string(line);
}

std::map<std::string, TokenType> Scanner::keywords
{
  {"and", TokenType::AND},
  {"class", TokenType::CLASS},
  {"else", TokenType::ELSE},
  {"false", TokenType::FALSE},
  {"for", TokenType::FOR},
  {"fun", TokenType::FUN},
  {"if", TokenType::IF},
  {"nil", TokenType::NIL},
  {"or", TokenType::OR},
  {"print", TokenType::PRINT},
  {"return", TokenType::RETURN},
  {"super", TokenType::SUPER},
  {"this", TokenType::THIS},
  {"true", TokenType::TRUE},
  {"var", TokenType::VAR},
  {"while", TokenType::WHILE},
};

bool Scanner::is_alpha(char c)
{
  return (std::isalpha(c) || c == '_');
}

bool Scanner::is_digit(char c)
{
  return std::isdigit(c);
}

bool Scanner::is_alnum(char c)
{
  return is_alpha(c) || is_digit(c);
}

bool Scanner::is_at_end()
{
  return m_current >= m_source.size();
}

char Scanner::advance()
{
  return m_source[m_current++];
}

bool Scanner::match(char expected)
{
  if (is_at_end())
  {
    return false;
  }
  if (m_source[m_current] != expected)
  {
    return false;
  }
  ++m_current;
  return true;
}

char Scanner::peek()
{
  if (is_at_end())
  {
    return '\0';
  }
  return m_source[m_current];
}

char Scanner::peek_next()
{
  if (m_current + 1 >= m_source.size())
  {
    return '\0';
  }
  return m_source[m_current + 1];
}

void Scanner::add_token(TokenType type)
{
  add_token(type, m_source.substr(m_start, m_current - m_start));
}

void Scanner::add_token(TokenType type, std::string lexeme)
{
  m_tokens.emplace_back(type, std::move(lexeme), m_line);
}


std::vector<Token> Scanner::scan_tokens()
{
  while (!is_at_end())
  {
    char c = advance();
    switch (c) {
      case '(': add_token(TokenType::LEFT_PAREN); break;
      case ')': add_token(TokenType::RIGHT_PAREN); break;
      case '{': add_token(TokenType::LEFT_BRACE); break;
      case '}': add_token(TokenType::RIGHT_BRACE); break;
      case ',': add_token(TokenType::COMMA); break;
      case '.': add_token(TokenType::DOT); break;
      case '-': add_token(TokenType::MINUS); break;
      case '+': add_token(TokenType::PLUS); break;
      case ';': add_token(TokenType::SEMICOLON); break;
      case '*': add_token(TokenType::STAR); break;
      case '!': add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
      case '=': add_token(match('=') ? TokenType::EQAUL_EQUAL : TokenType::EQUAL); break;
      case '<': add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
      case '>': add_token(match('=') ? TokenType::GREATER_EQUAL: TokenType::GREATER); break;
      case '/': {
                  if (match('/'))
                  {
                    while (peek() != '\n' && !is_at_end()) advance();
                  }
                  else
                  {
                    add_token(TokenType::SLASH);
                  }
                } break;
      case '"': {
                  while (peek() != '"' && !is_at_end())
                  {
                    if (peek() == '\n')
                    {
                      ++m_line;
                    }
                    advance();
                  }
                  if (is_at_end())
                  {
                    error(m_line, "Unterminated String");
                    continue;
                  }
                  advance();
                  add_token(TokenType::STRING, m_source.substr(m_start + 1, m_current - m_start - 2));
                } break;
      case ' ':
      case '\r':
      case '\t': break;
      case '\n': ++m_line; break;
      default : {
                  if (is_digit(c))
                  {
                    while (is_digit(peek()))
                    {
                      advance();
                    }
                    if (peek() == '.' && is_digit(peek_next()))
                    {
                      advance();
                      while (is_digit(peek()))
                      {
                        advance();
                      }
                    }
                    add_token(TokenType::NUMBER);
                  }
                  else if (is_alpha(c))
                  {
                    while (is_alnum(peek()))
                    {
                      advance();
                    }
                    std::string str = m_source.substr(m_start, m_current - m_start);
                    if (keywords.contains(str))
                    {
                      add_token(keywords[str], str);
                    }
                    else
                    {
                      add_token(TokenType::IDENTIFIER, str);
                    }
                  }
                  else
                  {
                    error(m_line, "Unexpected character");
                  }
                } break;
    }
    m_start = m_current;
  }
  return m_tokens;
}