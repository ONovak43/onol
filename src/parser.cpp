#include "parser.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>

#include "bytecode.hpp"
#include "debug.hpp"
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
       {bindMethod(&Parser::grouping), nullptr, Precedence::NONE}},
      {TokenType::RIGHT_PAREN, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::LEFT_BRACE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::RIGHT_BRACE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::COMMA, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::DOT, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::MINUS,
       {bindMethod(&Parser::unary), bindMethod(&Parser::binary),
        Precedence::TERM}},
      {TokenType::PLUS,
       {nullptr, bindMethod(&Parser::binary), Precedence::TERM}},
      {TokenType::SEMICOLON, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::COLON, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::SLASH,
       {nullptr, bindMethod(&Parser::binary), Precedence::FACTOR}},
      {TokenType::STAR,
       {nullptr, bindMethod(&Parser::binary), Precedence::FACTOR}},

      {TokenType::BANG,
       {bindMethod(&Parser::unary), nullptr, Precedence::NONE}},
      {TokenType::BANG_EQUAL,
       {nullptr, bindMethod(&Parser::binary), Precedence::EQUALITY}},
      {TokenType::EQUAL, {nullptr, nullptr, Precedence::ASSIGNMENT}},
      {TokenType::EQUAL_EQUAL,
       {nullptr, bindMethod(&Parser::binary), Precedence::EQUALITY}},
      {TokenType::GREATER,
       {nullptr, bindMethod(&Parser::binary), Precedence::COMPARISON}},
      {TokenType::GREATER_EQUAL,
       {nullptr, bindMethod(&Parser::binary), Precedence::COMPARISON}},
      {TokenType::LESS,
       {nullptr, bindMethod(&Parser::binary), Precedence::COMPARISON}},
      {TokenType::LESS_EQUAL,
       {nullptr, bindMethod(&Parser::binary), Precedence::COMPARISON}},

      {TokenType::IDENTIFIER, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::STRING,
       {bindMethod(&Parser::string), nullptr, Precedence::NONE}},
      {TokenType::INTEGER,
       {bindMethod(&Parser::number), nullptr, Precedence::NONE}},
      {TokenType::DOUBLE,
       {bindMethod(&Parser::number), nullptr, Precedence::NONE}},
      {TokenType::TYPE_STRING, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::TYPE_INTEGER, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::TYPE_DOUBLE, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::TYPE_BOOL, {nullptr, nullptr, Precedence::NONE}},

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
       {bindMethod(&Parser::literal), nullptr, Precedence::NONE}},
      {TokenType::FALSE,
       {bindMethod(&Parser::literal), nullptr, Precedence::NONE}},
      {TokenType::NUL,
       {bindMethod(&Parser::literal), nullptr, Precedence::NONE}},
      {TokenType::THIS, {nullptr, nullptr, Precedence::NONE}},

      {TokenType::ERROR, {nullptr, nullptr, Precedence::NONE}},
      {TokenType::END, {nullptr, nullptr, Precedence::NONE}},
  };
};

const ParseRule& Parser::getRule(TokenType type) {
  return rules.at(type);
}

void Parser::expression() {
  parsePrecedence(Precedence::ASSIGNMENT);
}

void Parser::grouping() {
  expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void Parser::binary() {
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

void Parser::unary() {
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

void Parser::number() {
  if (previous->type != TokenType::INTEGER &&
      previous->type != TokenType::DOUBLE) {
    error("Invalid numeric literal.");
  }
  emitConstant(previous->literal.value());
}

void Parser::string() {
  emitConstant(previous->literal.value());
}

void Parser::literal() {
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

void Parser::emitByte(OpCode byte) {
  compilingCode()->putOpCode(byte, previous->line);
}

void Parser::emitConstant(const Type& value) {
  compilingCode()->putConstant(value, previous->line);
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

  while (current->type != TokenType::END) {
    if (previous->type == TokenType::SEMICOLON) return;

    switch (current->type) {
      case TokenType::STRUCT:
      case TokenType::FN:
      case TokenType::LET:
      case TokenType::TYPE_BOOL:
      case TokenType::TYPE_DOUBLE:
      case TokenType::TYPE_INTEGER:
      case TokenType::TYPE_STRING:
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

  if (token->type == TokenType::END) {
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
      expression();
      consume(TokenType::END, "Expected end of expression.");
      endParse();
      break;
    } catch (const InterpreterError& ex) {
      errored = true;
      std::cerr << ex.what() << "\n";
      if (current && current->type == TokenType::END) {
        break;
      }
      synchronize();
    }
  }
  return !hadError();
}
