#include "parser.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

#include "bytecode.hpp"
#include "debug.hpp"
#include "interned_strings.hpp"
#include "token.hpp"

Parser::Parser() {
  initializeRules();
}

void Parser::initializeRules() {
  auto bindMethod = [this](void (Parser::*method)()) -> ParseFn {
    return [this, method]() { (this->*method)(); };
  };

  rules = std::unordered_map<TokenType, ParseRule>{
      {TokenType::LEFT_PAREN,
       {bindMethod(&Parser::parseGroup), nullptr, Precedence::NONE}},
      {TokenType::RIGHT_PAREN, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::LEFT_BRACE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::RIGHT_BRACE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::COMMA, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::DOT, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::MINUS,
       {bindMethod(&Parser::parseUnaryExpr),
        bindMethod(&Parser::parseBinaryExpr), Precedence::TERM}},
      {TokenType::PLUS,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::TERM}},
      {TokenType::SEMICOLON, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::COLON, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::SLASH,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::FACTOR}},
      {TokenType::STAR,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::FACTOR}},

      {TokenType::BANG,
       {bindMethod(&Parser::parseUnaryExpr), nullptr, Precedence::NONE}},
      {TokenType::BANG_EQUAL,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::EQUALITY}},
      {TokenType::EQUAL, {nullptr, nullptr, Precedence::ASSIGNMENT}},
      {TokenType::EQUAL_EQUAL,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::EQUALITY}},
      {TokenType::GREATER,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::COMPARISON}},
      {TokenType::GREATER_EQUAL,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::COMPARISON}},
      {TokenType::LESS,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::COMPARISON}},
      {TokenType::LESS_EQUAL,
       {nullptr, bindMethod(&Parser::parseBinaryExpr), Precedence::COMPARISON}},

      {TokenType::IDENTIFIER,
       {bindMethod(&Parser::var), nullptr, Precedence::NONE}},
      {TokenType::STRING,
       {bindMethod(&Parser::parseString), nullptr, Precedence::NONE}},
      {TokenType::INTEGER,
       {bindMethod(&Parser::parseNumber), nullptr, Precedence::NONE}},
      {TokenType::DOUBLE,
       {bindMethod(&Parser::parseNumber), nullptr, Precedence::NONE}},
      {TokenType::LET_STRING, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::LET_INTEGER, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::LET_DOUBLE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::LET_BOOL, {nullptr, nullptr, Precedence::NONE}},

      {TokenType::INTERFACE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::STRUCT, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::FN, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::MUT, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::LET, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::FOR, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::IN, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::RETURN, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::RETURNIF, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::IF, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::ELSE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::OR, {nullptr, nullptr, Precedence::OR}},
      {TokenType::AND, {nullptr, nullptr, Precedence::AND}},
      {TokenType::TRUE,
       {bindMethod(&Parser::parseLiteral), nullptr, Precedence::NONE}},
      {TokenType::FALSE,
       {bindMethod(&Parser::parseLiteral), nullptr, Precedence::NONE}},
      {TokenType::NUL,
       {bindMethod(&Parser::parseLiteral), nullptr, Precedence::NONE}},
      {TokenType::THIS, {nullptr, nullptr, Precedence::NONE}},

      {TokenType::ERROR, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::TEOF, {nullptr, nullptr, Precedence::NONE}},
  };
};

const ParseRule& Parser::getRule(TokenType type) {
  return rules.at(type);
}

void Parser::parseDecl() {
  if (isVarDecl()) {
    parseVarDecl();
  } else {
    parseStmt();
  }
}

void Parser::var() {
  namedVar(previous);
}

void Parser::namedVar(const std::unique_ptr<Token>& token) {
  std::size_t arg = identifierConst(token);
  if (match(TokenType::EQUAL)) {
    emitVariableByte(OpCode::SET_GLOBAL, OpCode::SET_GLOBAL_LONG, arg);
  } else {
    emitVariableByte(OpCode::GET_GLOBAL, OpCode::GET_GLOBAL_LONG, arg);
  }
}

bool Parser::isVarDecl() {
  return match(TokenType::LET) || match(TokenType::LET_BOOL) ||
         match(TokenType::LET_DOUBLE) || match(TokenType::LET_STRING) ||
         match(TokenType::LET_INTEGER);
}

void Parser::parseVarDecl() {
  bool deducible = !checkPrev(TokenType::LET);
  auto globalVar = parseVar("Expect variable name");

  if (match(TokenType::EQUAL)) {
    parseExpr();
  } else if (deducible) {
    emitDefaultVarValue();
  } else {
    errorAtCurrent("Declaration of variable '" + std::string(current->lexeme) +
                   "' with deduce type 'let' requires an initializer.");
  }

  defineVar(globalVar);
}

std::size_t Parser::parseVar(std::string_view errorMessage) {
  consume(TokenType::IDENTIFIER, errorMessage);
  return identifierConst(previous);
}

std::size_t Parser::identifierConst(const std::unique_ptr<Token>& token) {
  ObjString* name = getOrIntern(token->lexeme);
  return compilingCode()->createConstant(name);
}

void Parser::defineVar(std::size_t globalAddress) {
  emitVariableByte(OpCode::DEFINE_GLOBAL, OpCode::DEFINE_GLOBAL_LONG,
                   globalAddress);
}

void Parser::emitDefaultVarValue() {
  if (checkPrev(TokenType::LET_BOOL)) {
    emitByte(OpCode::FALSE);
  } else if (checkPrev(TokenType::LET_INTEGER)) {
    emitConstant(0);
  } else if (checkPrev(TokenType::LET_DOUBLE)) {
    emitConstant(0.0);
  } else {
    ObjString* emptyString = getOrIntern("");
    emitConstant(emptyString);
  }
}

void Parser::parseStmt() {
  parseExpr();
}

void Parser::parseExprStmt() {
  parseExpr();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  emitByte(OpCode::POP);
}

void Parser::parseExpr() {
  parsePrecedence(Precedence::ASSIGNMENT);
}

void Parser::parseGroup() {
  parseExpr();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void Parser::parseBinaryExpr() {
  TokenType opType = previous->type;
  const auto& rule = getRule(opType);
  parsePrecedence(
      static_cast<Precedence>(static_cast<size_t>(rule.precedence) + 1));

  switch (opType) {
    case TokenType::PLUS: {
      emitByte(OpCode::ADD);
      break;
    }
    case TokenType::MINUS: {
      emitByte(OpCode::SUBTRACT);
      break;
    }
    case TokenType::STAR: {
      emitByte(OpCode::MULTIPLY);
      break;
    }
    case TokenType::SLASH: {
      emitByte(OpCode::DIVIDE);
      break;
    }
    case TokenType::BANG_EQUAL: {
      emitByte(OpCode::NOT_EQUAL);
      break;
    }
    case TokenType::EQUAL_EQUAL: {
      emitByte(OpCode::EQUAL);
      break;
    }
    case TokenType::GREATER: {
      emitByte(OpCode::GREATER);
      break;
    }
    case TokenType::GREATER_EQUAL: {
      emitByte(OpCode::GREATER_EQUAL);
      break;
    }
    case TokenType::LESS: {
      emitByte(OpCode::LESS);
      break;
    }
    case TokenType::LESS_EQUAL: {
      emitByte(OpCode::LESS_EQUAL);
      break;
    }
    default:
      return;
  }
}

void Parser::parseUnaryExpr() {
  TokenType opType = previous->type;

  parsePrecedence(Precedence::UNARY);

  switch (opType) {
    case TokenType::MINUS: {
      emitByte(OpCode::NEGATE);
      break;
    }
    case TokenType::BANG: {
      emitByte(OpCode::NOT);
      break;
    }
    default: {
      return;
    }
  }
}

void Parser::parseNumber() {
  if (previous->type != TokenType::INTEGER &&
      previous->type != TokenType::DOUBLE) {
    error("Invalid numeric literal.");
  }
  emitConstant(previous->literal.value());
}

void Parser::parseString() {
  emitConstant(previous->literal.value());
}

void Parser::parseLiteral() {
  switch (previous->type) {
    case TokenType::FALSE: {
      emitByte(OpCode::FALSE);
      break;
    }
    case TokenType::TRUE: {
      emitByte(OpCode::TRUE);
      break;
    }
    case TokenType::NUL: {
      emitByte(OpCode::NUL);
      break;
    }
    default: {
      return;
    }
  }
}

void Parser::emitVariableByte(OpCode shortCode, OpCode longCode,
                              std::size_t address) {
  if (address < MAX_CONSTANT_POOL_ADDRESS_LENGTH) {
    emitByte(shortCode);
    emitByte(static_cast<uint8_t>(address));
  } else {
    emitByte(longCode);
    emitByte(address);
  }
}

void Parser::emitByte(OpCode byte) {
  compilingCode()->putOpCode(byte, previous->line);
}

void Parser::emitConstant(const Type& value) {
  compilingCode()->putConstant(value, previous->line);
}

void Parser::emitByte(uint8_t byte) {
  compilingCode()->putRaw(byte, previous->line);
}

void Parser::emitByte(std::size_t byte) {
  compilingCode()->putRaw(byte, previous->line);
}

std::shared_ptr<Bytecode> Parser::compilingCode() {
  return compilingBytecode;
}

void Parser::next() {
  previous = std::move(current);

  while (true) {
    current = std::make_unique<Token>(tokenizer->scanToken());
    if (current->type != TokenType::ERROR) {
      break;
    }

    errorAtCurrent(current->lexeme);
  }
}

bool Parser::match(TokenType type) {
  if (!checkCurrent(type)) {
    return false;
  }
  next();
  return true;
}

bool Parser::checkCurrent(TokenType type) {
  return current->type == type;
}

bool Parser::checkPrev(TokenType type) {
  return previous->type == type;
}

void Parser::consume(TokenType type, std::string_view message) {
  if (current->type == type) {
    next();
    return;
  }

  errorAtCurrent(message);
}

void Parser::synchronize() {
  try {
    next();
  } catch (const InterpreterError&) {
  }

  while (!checkCurrent(TokenType::TEOF)) {
    if (checkPrev(TokenType::SEMICOLON)) return;

    switch (current->type) {
      case TokenType::STRUCT:
      case TokenType::FN:
      case TokenType::LET:
      case TokenType::LET_BOOL:
      case TokenType::LET_DOUBLE:
      case TokenType::LET_INTEGER:
      case TokenType::LET_STRING:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::RETURN:
      case TokenType::RETURNIF:
        return;
      default:
        break;
    }

    try {
      next();
    } catch (const InterpreterError&) {
    }
  }
}

bool Parser::hadError() {
  return errored;
}

void Parser::errorAtCurrent(std::string_view message) {
  errorAt(previous.get(), message);
}

void Parser::error(std::string_view message) {
  errorAt(previous.get(), message);
}

void Parser::errorAt(const Token* token, std::string_view message) {
  std::ostringstream errorMsg;
  errorMsg << "Error";

  if (token->type == TokenType::TEOF) {
    errorMsg << " at end";
  } else if (token->type == TokenType::ERROR) {
  } else {
    errorMsg << " at '" << token->lexeme << "'";
  }

  errorMsg << ": " << message;
  throw ParseError(token->line, errorMsg.str());
}

void Parser::emitReturn() {
  emitByte(OpCode::RETURN);
}

void Parser::endParse() {
  emitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!errored) {
    disassembleBytecode(*compilingCode(), "code");
  }
#endif
}

void Parser::parsePrecedence(Precedence precedence) {
  next();
  ParseFn prefixRule = getRule(previous->type).prefix;

  if (prefixRule == nullptr) {
    error("Expect expression.");
    return;
  }

  prefixRule();

  while (precedence <= getRule(current->type).precedence) {
    next();
    ParseFn infixRule = getRule(previous->type).infix;
    infixRule();
  }
}

bool Parser::parse(std::string_view sourceCode,
                   std::shared_ptr<Bytecode> bytecode) {
  errored = false;
  tokenizer = std::make_unique<Tokenizer>(sourceCode);
  this->compilingBytecode = bytecode;

  while (true) {
    try {
      next();

      while (!match(TokenType::TEOF)) {
        parseDecl();
      }

      endParse();
      break;
    } catch (const InterpreterError& ex) {
      errored = true;
      std::cerr << ex.what() << "\n";
      if (current && current->type == TokenType::TEOF) {
        break;
      }
      synchronize();
    }
  }
  return !hadError();
}
