#pragma once

#include <functional>
#include <memory>
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
  static constexpr uint16_t MAX_CONSTANT_POOL_ADDRESS_LENGTH = 256;
  std::shared_ptr<Bytecode> compilingBytecode;
  std::unique_ptr<Tokenizer> tokenizer;
  std::unique_ptr<Token> current;
  std::unique_ptr<Token> previous;
  bool errored = false;

  void parseDecl();
  void parseVarDecl();
  void parseStmt();
  void parseExprStmt();
  void parseExpr();
  void parseGroup();
  void parseBinaryExpr();
  void parseUnaryExpr();
  void parseNumber();
  void parseString();
  void parseLiteral();

  void var();
  void namedVar(const std::unique_ptr<Token>& token);

  void next();
  bool match(TokenType type);
  bool checkCurrent(TokenType type);
  bool checkPrev(TokenType type);
  bool isVarDecl();
  std::size_t parseVar(std::string_view errorMessage);
  std::size_t identifierConst(const std::unique_ptr<Token>& token);
  void defineVar(std::size_t globalAddress);

  void consume(TokenType type, std::string_view message);
  void synchronize();

  bool hadError();
  void errorAtCurrent(std::string_view message);
  void error(std::string_view message);
  void errorAt(const Token* token, std::string_view message);

  void emitByte(OpCode byte);
  void emitByte(uint8_t byte);
  void emitByte(std::size_t byte);
  void emitVariableByte(OpCode shortCode, OpCode longCode, std::size_t address);
  void emitConstant(const Type& value);
  void emitReturn();
  void emitDefaultVarValue();

  void endParse();
  std::shared_ptr<Bytecode> compilingCode();

  void parsePrecedence(Precedence precedence);
  const ParseRule& getRule(TokenType type);
  void initializeRules();

  std::unordered_map<TokenType, ParseRule> rules;

 public:
  Parser();
  bool parse(std::string_view sourceCode, std::shared_ptr<Bytecode> bytecode);
};
