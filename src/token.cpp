#include "token.hpp"

std::unordered_map<std::string, TokenType> Token::keywords = {
    {"and", TokenType::AND},
    {"bool", TokenType::LET_BOOL},
    {"double", TokenType::LET_DOUBLE},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fn", TokenType::FN},
    {"in", TokenType::IN},
    {"int", TokenType::LET_INTEGER},
    {"interface", TokenType::INTERFACE},
    {"if", TokenType::IF},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"nil", TokenType::NUL},
    {"or", TokenType::OR},
    {"return", TokenType::RETURN},
    {"returnif", TokenType::RETURNIF},
    {"string", TokenType::LET_STRING},
    {"struct", TokenType::STRUCT},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
};

Token::Token(int line, TokenType type, std::string_view lexeme,
             std::optional<Type> literal)
    : type(type), lexeme(lexeme), literal(literal), line(line) {
}

Token Token::lookup(std::string_view lexeme, int line) {
  auto keywordType = keywords.find(std::string(lexeme));
  if (keywordType != keywords.end()) {
    switch (keywordType->second) {
      case TokenType::TRUE:
        return Token(line, TokenType::TRUE, lexeme,
                     std::make_optional<Type>(true));
      case TokenType::FALSE:
        return Token(line, TokenType::FALSE, lexeme,
                     std::make_optional<Type>(false));
      default:
        return Token(line, keywordType->second, lexeme);
    }
  }
  return Token(line, TokenType::IDENTIFIER, lexeme);
}
