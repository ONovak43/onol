#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <sstream>
#include <string_view>
#include <unordered_map>

#include "bytecode.hpp"
#include "interpreter_error.hpp"
#include "token.hpp"
#include "tokenizer.hpp"

enum class Precedence {
  NONE,
  ASSIGNMENT,
  OR,
  AND,
  EQUALITY,
  COMPARISON,
  TERM,
  FACTOR,
  UNARY,
  CALL,
  PRIMARY
};

class Parser;

using ParseFn = std::function<void()>;

struct ParseRule {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
};

class ParseError : public InterpreterError {
 public:
  ParseError() = delete;
  ParseError(uint32_t line, const std::string& msg)
      : InterpreterError(line, msg) {
  }
};

class Parser {
 private:
  std::shared_ptr<Bytecode> compilingBytecode;
  std::unique_ptr<Tokenizer> tokenizer;
  std::unique_ptr<Token> current;
  std::unique_ptr<Token> previous;
  bool errored = false;

  void expression();
  void grouping();
  void binary();
  void unary();
  void number();
  void string();
  void literal();

  void next();

  bool hadError();
  void errorAtCurrent(std::string_view message);
  void error(std::string_view message);
  void errorAt(const Token* token, std::string_view message);
  void emitByte(OpCode byte);
  void emitConstant(const Type& value);
  void emitReturn();
  void endParse();
  std::shared_ptr<Bytecode> compilingCode();
  void parsePrecedence(Precedence precedence);
  const ParseRule& getRule(TokenType type);
  void initializeRules();
  void consume(TokenType type, std::string_view message);
  void synchronize();

  std::unordered_map<TokenType, ParseRule> rules;

 public:
  Parser();
  bool parse(std::string_view sourceCode, std::shared_ptr<Bytecode> bytecode);
};
