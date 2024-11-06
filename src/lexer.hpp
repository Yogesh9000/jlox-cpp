#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#define TokenFunc(Func) \
        Func(LEFT_PAREN) \
        Func(RIGHT_PAREN) \
        Func(LEFT_BRACE) \
        Func(RIGHT_BRACE) \
        Func(COMMA) \
        Func(DOT) \
        Func(MINUS) \
        Func(PLUS) \
        Func(SEMICOLON) \
        Func(SLASH) \
        Func(STAR) \
        Func(BANG)\
        Func(BANG_EQUAL)\
        Func(EQUAL)\
        Func(EQAUL_EQUAL)\
        Func(GREATER)\
        Func(GREATER_EQUAL)\
        Func(LESS)\
        Func(LESS_EQUAL)\
        Func(IDENTIFIER)\
        Func(STRING)\
        Func(NUMBER)\
        Func(AND)\
        Func(CLASS)\
        Func(ELSE)\
        Func(FALSE)\
        Func(FUN)\
        Func(FOR)\
        Func(IF)\
        Func(NIL)\
        Func(OR)\
        Func(PRINT)\
        Func(RETURN)\
        Func(SUPER)\
        Func(THIS)\
        Func(TRUE)\
        Func(VAR)\
        Func(WHILE)\
        Func(eof)

#define GenEnum(E) E ,
#define GenEnumStr(E) case TokenType::E: return #E; break; 
        
enum class TokenType
{
  TokenFunc(GenEnum)
};

std::string to_string(TokenType type);


struct Token {
  TokenType type;
  std::string lexeme;
  int line;

  Token(TokenType type, std::string lexeme, int line)
      : type(type), lexeme(std::move(lexeme)), line(line) 
  {
  }

  [[nodiscard]] std::string to_string() const;
};

struct Scanner
{

public:
  explicit Scanner(std::string source)
    : m_source(std::move(source))
  {
  }

  std::vector<Token> scan_tokens();

private:
  bool is_at_end();
  char advance();
  bool match(char expected);
  char peek();
  char peek_next();
  void add_token(TokenType type);
  void add_token(TokenType type, std::string lexeme);

  bool is_alpha(char c);
  bool is_digit(char c);
  bool is_alnum(char c);

  std::string m_source;
  std::vector<Token> m_tokens;
  int m_start{0};
  int m_current{0};
  int m_line{0};

  static std::map<std::string, TokenType> keywords;
};
