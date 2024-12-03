#pragma once

#include <sstream>
#include <string_view>

#include "token.hpp"
#include "types.hpp"

class ScanError : public std::exception {
 private:
  uint32_t line;
  std::string message;
  std::string formattedMessage;

 public:
  ScanError() = delete;
  ScanError(uint32_t line, const std::string& msg) : line(line), message(msg) {
    std::ostringstream oss;
    oss << "[line " << line << "] Error: " << message;
    formattedMessage = oss.str();
  }

  virtual const char* what() const noexcept override {
    return formattedMessage.c_str();
  }

  uint32_t getLine() const noexcept {
    return line;
  }
  const std::string& getMessage() const noexcept {
    return message;
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
