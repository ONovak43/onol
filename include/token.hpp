#pragma once

#include <optional>
#include <unordered_map>

#include "types.hpp"

enum class TokenType {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  COLON,
  SLASH,
  STAR,

  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  IDENTIFIER,
  STRING,
  INTEGER,
  DOUBLE,
  BOOL,
  TYPE_STRING,
  TYPE_INTEGER,
  TYPE_DOUBLE,
  TYPE_BOOL,

  INTERFACE,
  STRUCT,
  FN,
  MUT,
  LET,
  FOR,
  IN,
  RETURN,
  RETURNIF,
  IF,
  ELSE,
  OR,
  AND,
  TRUE,
  FALSE,
  NIL,
  THIS,

  ERROR,
  END
};

class Token {
 private:
  static std::unordered_map<std::string, TokenType> keywords;

 public:
  const TokenType type;
  std::string_view lexeme;
  std::optional<Type> literal = std::nullopt;
  int line;

  Token(int line, TokenType type, std::string_view lexeme,
        std::optional<Type> literal = std::nullopt);

  static Token lookup(std::string_view lexeme, int line);
};