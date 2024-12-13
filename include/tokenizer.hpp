#pragma once

#include <sstream>
#include <string_view>

#include "interpreter_error.hpp"
#include "token.hpp"
#include "types.hpp"

class ScanError : public InterpreterError {
 public:
  ScanError() = delete;
  ScanError(uint32_t line, const std::string& msg)
      : InterpreterError(line, msg) {
  }
};

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
  void skipWhitespace();
  bool shouldInsertSemicolon();
  Token string();
  Token number();
  Token identifier();

  char next();
  bool match(char expected);
  char peek();
  char peekNext();

 public:
  explicit Tokenizer(std::string_view sourceCode) : source(sourceCode) {
  }

  Token scanToken();
};
