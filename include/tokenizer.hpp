#pragma once

#include <string_view>

#include "token.hpp"
#include "types.hpp"

class Tokenizer {
 private:
  std::string_view source;
  std::size_t start = 0;
  std::size_t current = 0;
  int line = 1;
  TokenType previousTokenType;
  bool insertSemicolon = false;

  bool isAtEnd();
  char currentChar();
  Token makeToken(TokenType type);
  Token makeToken(std::size_t line, TokenType type, std::string_view lexeme);
  Token makeToken(TokenType type, std::string_view lexeme, const Type& literal);
  Token errorToken(const std::string& msg);
  bool isWhitespaceEndStatement();
  bool handleNewLine();
  void skipWhitespace();
  bool shouldInsertSemicolon();
  Token string();
  Token number();
  Token identifier();
  TokenType identifierType();
  TokenType checkKeyword(std::size_t start, std::size_t length,
                         const std::string& rest, TokenType type);
  char next();
  bool match(char expected);
  char peek();
  char peekNext();

 public:
  Tokenizer(std::string_view sourceCode) : source(sourceCode) {
  }

  Token scanToken();
};
