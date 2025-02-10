#include "tokenizer.hpp"

#include <charconv>
#include <optional>
#include <stdexcept>
#include <string_view>

#include "allocator.hpp"
#include "interned_strings.hpp"
#include "token.hpp"

static bool isDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

static bool isAlpha(char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_';
}

template <typename T>
static T parseNumericLiteral(std::string_view lexeme, int line) {
  T literal;
  auto result =
      std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), literal);
  if (result.ec == std::errc()) {
    return literal;
  } else if (result.ec == std::errc::invalid_argument) {
    throw ScanError(line,
                    "Invalid numeric value. Lexeme: " + std::string(lexeme));
  } else if (result.ec == std::errc::result_out_of_range) {
    throw ScanError(
        line, "Numeric value is out of range for the target type. Lexeme: " +
                  std::string(lexeme));
  } else {
    throw ScanError(line, "Unknown error parsing numeric value. Lexeme: " +
                              std::string(lexeme));
  }
}

bool Tokenizer::isAtEnd() {
  return current >= source.size();
}

char Tokenizer::currentChar() {
  return source[current];
}

Token Tokenizer::makeToken(TokenType type) {
  std::string_view lexeme = source.substr(start, current - start);
  previousTokenType = type;
  return Token(line, type, lexeme);
}

Token Tokenizer::makeToken(std::size_t line, TokenType type,
                           std::string_view lexeme) {
  previousTokenType = type;
  return Token(line, type, lexeme);
}

Token Tokenizer::makeToken(TokenType type, std::string_view lexeme,
                           const Type& literal) {
  previousTokenType = type;
  return Token(line, type, lexeme, literal);
}

Token Tokenizer::errorToken(const std::string& msg) {
  return Token(line, TokenType::ERROR, msg);
}

char Tokenizer::next() {
  return source[current++];
}

bool Tokenizer::match(char expected) {
  if (isAtEnd()) {
    return false;
  }

  char test = currentChar();

  if (test != expected) {
    return false;
  }

  current++;
  return true;
}

char Tokenizer::peek() {
  return source[current];
}

char Tokenizer::peekNext() {
  if (isAtEnd()) return '\0';
  return source[current + 1];
}

bool Tokenizer::shouldInsertSemicolon() {
  switch (previousTokenType) {
    case TokenType::IDENTIFIER:
    case TokenType::INTEGER:
    case TokenType::DOUBLE:
    case TokenType::STRING:
    case TokenType::TRUE:
    case TokenType::FALSE:
    case TokenType::RETURN:
    case TokenType::RIGHT_PAREN:
    case TokenType::RIGHT_BRACE:
      return true;
    default:
      return false;
  }
}

Token Tokenizer::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }
    next();
  }

  if (isAtEnd()) {
    return errorToken("Unterminated string.");
  }

  next();
  std::string_view lexeme = source.substr(start, current - start);
  std::string_view literal =
      source.substr(start + 1, current - start - 2);  // remove "

  ObjString* interned = getOrIntern(literal);

  return makeToken(TokenType::STRING, lexeme, interned);
}

Token Tokenizer::number() {
  while (isDigit(peek())) {
    next();
  }

  bool isDouble = false;
  if (peek() == '.' && isDigit(peekNext())) {
    isDouble = true;
    next();

    while (isDigit(peek())) {
      next();
    }
  }

  std::string_view lexeme = source.substr(start, current - start);
  if (isDouble) {
    return makeToken(TokenType::DOUBLE, lexeme,
                     parseNumericLiteral<double>(lexeme, line));
  } else {
    return makeToken(TokenType::INTEGER, lexeme,
                     parseNumericLiteral<int32_t>(lexeme, line));
  }
}

Token Tokenizer::identifier() {
  while (isAlpha(peek()) || isDigit(peek())) {
    next();
  }

  std::string_view lexeme = source.substr(start, current - start);
  return Token::lookup(lexeme, line);
}

void Tokenizer::skipWhitespace() {
  for (;;) {
    char c = peek();

    switch (c) {
      case ' ':
      case '\t':
      case '\r':
        next();
        break;
      case '\n':
        line++;
        if (insertSemicolon) {
          return;
        }
        next();
        break;
      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) {
            next();
          }
        } else if (peekNext() == '*') {
          next();
          next();
          while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
            if (peek() == '\n') {
              line++;
            }
            next();
          }
        }
        return;
      default:
        return;
    }
  }
}

Token Tokenizer::scanToken() {
  skipWhitespace();

  start = current;

  if (isAtEnd()) {
    return makeToken(TokenType::TEOF);
  }

  insertSemicolon = false;
  char ch = next();

  if (isAlpha(ch)) {
    Token token = identifier();
    switch (token.type) {
      case TokenType::IDENTIFIER:
      case TokenType::TRUE:
      case TokenType::FALSE:
      case TokenType::RETURN:
      case TokenType::NUL:
      case TokenType::THIS:
        insertSemicolon = true;
        break;
      default:
        insertSemicolon = false;
        break;
    }
    return token;
  }

  if (isDigit(ch)) {
    insertSemicolon = true;
    return number();
  }

  switch (ch) {
    case '\n': {  // semicolon autoinsert
      insertSemicolon = false;
      std::string_view lexeme = source.substr(start, current - start);
      return makeToken(line - 1, TokenType::SEMICOLON, lexeme);
    }
    case '(':
      return makeToken(TokenType::LEFT_PAREN);
    case ')':
      insertSemicolon = true;
      return makeToken(TokenType::RIGHT_PAREN);
    case '{':
      return makeToken(TokenType::LEFT_BRACE);
    case '}':
      insertSemicolon = true;
      return makeToken(TokenType::RIGHT_BRACE);
    case ';':
      return makeToken(TokenType::SEMICOLON);
    case ':':
      return makeToken(TokenType::COLON);
    case ',':
      return makeToken(TokenType::COMMA);
    case '.':
      return makeToken(TokenType::DOT);
    case '-':
      return makeToken(TokenType::MINUS);
    case '+':
      return makeToken(TokenType::PLUS);
    case '/':
      return makeToken(TokenType::SLASH);
    case '*':
      return makeToken(TokenType::STAR);
    case '!':
      return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    case '=':
      return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    case '<':
      return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    case '>':
      return makeToken(match('=') ? TokenType::GREATER_EQUAL
                                  : TokenType::GREATER);
    case '"':
      insertSemicolon = true;
      return string();
  }

  return errorToken("Unexpected character.");
}
