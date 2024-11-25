#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "tokenizer.hpp"

void skipTokens(Tokenizer& tokenizer, int count) {
  for (int i = 0; i < count; i++) {
    tokenizer.scanToken();
  }
}

std::vector<Token> tokens = {
    {1, TokenType::STRING, "\"Hello, World!\"",
     std::make_optional<Type>(std::string("Hello, World!"))},
    {2, TokenType::INTEGER, "123", std::make_optional<Type>(123)},
    {3, TokenType::DOUBLE, "3.1415", std::make_optional<Type>(3.1415)},
    {4, TokenType::NIL, "nil"},

    {5, TokenType::PLUS, "+"},
    {6, TokenType::MINUS, "-"},
    {7, TokenType::STAR, "*"},
    {8, TokenType::SLASH, "/"},
    {9, TokenType::EQUAL, "="},
    {10, TokenType::EQUAL_EQUAL, "=="},
    {11, TokenType::BANG, "!"},
    {12, TokenType::BANG_EQUAL, "!="},
    {13, TokenType::GREATER, ">"},
    {14, TokenType::GREATER_EQUAL, ">="},
    {15, TokenType::LESS, "<"},
    {16, TokenType::LESS_EQUAL, "<="},
    {17, TokenType::LEFT_PAREN, "("},
    {18, TokenType::RIGHT_PAREN, ")"},
    {19, TokenType::LEFT_BRACE, "{"},
    {20, TokenType::RIGHT_BRACE, "}"},
    {21, TokenType::COMMA, ","},
    {22, TokenType::DOT, "."},
    {23, TokenType::SEMICOLON, ";"},
    {24, TokenType::COLON, ":"},

    {25, TokenType::AND, "and"},
    {26, TokenType::OR, "or"},
    {27, TokenType::IF, "if"},
    {28, TokenType::ELSE, "else"},
    {29, TokenType::FOR, "for"},
    {30, TokenType::IN, "in"},
    {31, TokenType::RETURN, "return"},
    {32, TokenType::RETURNIF, "returnif"},
    {33, TokenType::LET, "let"},
    {34, TokenType::MUT, "mut"},
    {35, TokenType::FN, "fn"},
    {36, TokenType::STRUCT, "struct"},
    {37, TokenType::INTERFACE, "interface"},
    {38, TokenType::TYPE_BOOL, "bool"},
    {39, TokenType::TYPE_INTEGER, "int"},
    {40, TokenType::TYPE_DOUBLE, "double"},
    {41, TokenType::TYPE_STRING, "string"},
    {42, TokenType::TRUE, "true", std::make_optional<Type>(true)},
    {43, TokenType::FALSE, "false", std::make_optional<Type>(false)},
    {44, TokenType::THIS, "this"},
    {45, TokenType::NIL, "nil"}};

std::string whitespace = " \t\n";

std::string generateTestSource() {
  std::string src;
  for (const auto& token : tokens) {
    src += std::string(token.lexeme) + whitespace;
  }
  return src;
}

TEST_CASE("Tokenizer correctly scans tokens") {
  auto src = generateTestSource();
  Tokenizer sut(src);

  std::unique_ptr<Token> token = std::make_unique<Token>(sut.scanToken());

  int prevLine = 1;

  for (auto i = 0; i < tokens.size() && token && token->type != TokenType::END;
       token = std::make_unique<Token>(sut.scanToken()), ++i) {
    Token expected = tokens[i];
    if (token->line != expected.line) {
      token = std::make_unique<Token>(sut.scanToken());
      prevLine = expected.line;
    }

    CHECK(expected.line == token->line);
    CHECK(expected.type == token->type);
    CHECK(expected.lexeme == token->lexeme);
    CHECK(expected.literal == token->literal);
  }
}

struct SemicolonTest {
  std::string input;
  std::string expectedTokens;
};

std::string tokenTypeToString(TokenType type) {
  switch (type) {
    case TokenType::LEFT_PAREN:
      return "(";
    case TokenType::RIGHT_PAREN:
      return ")";
    case TokenType::LEFT_BRACE:
      return "{";
    case TokenType::RIGHT_BRACE:
      return "}";
    case TokenType::COMMA:
      return ",";
    case TokenType::DOT:
      return ".";
    case TokenType::MINUS:
      return "-";
    case TokenType::PLUS:
      return "+";
    case TokenType::SEMICOLON:
      return ";";
    case TokenType::COLON:
      return ":";
    case TokenType::SLASH:
      return "/";
    case TokenType::STAR:
      return "*";
    case TokenType::BANG:
      return "!";
    case TokenType::BANG_EQUAL:
      return "!=";
    case TokenType::EQUAL:
      return "=";
    case TokenType::EQUAL_EQUAL:
      return "==";
    case TokenType::GREATER:
      return ">";
    case TokenType::GREATER_EQUAL:
      return ">=";
    case TokenType::LESS:
      return "<";
    case TokenType::LESS_EQUAL:
      return "<=";
    case TokenType::IDENTIFIER:
      return "IDENT";
    case TokenType::STRING:
      return "STRING";
    case TokenType::INTEGER:
      return "INTEGER";
    case TokenType::DOUBLE:
      return "DOUBLE";
    case TokenType::BOOL:
      return "BOOL";
    case TokenType::TYPE_STRING:
      return "string";
    case TokenType::TYPE_INTEGER:
      return "int";
    case TokenType::TYPE_DOUBLE:
      return "double";
    case TokenType::TYPE_BOOL:
      return "bool";
    case TokenType::INTERFACE:
      return "interface";
    case TokenType::STRUCT:
      return "struct";
    case TokenType::FN:
      return "fn";
    case TokenType::MUT:
      return "mut";
    case TokenType::LET:
      return "let";
    case TokenType::FOR:
      return "for";
    case TokenType::IN:
      return "in";
    case TokenType::RETURN:
      return "return";
    case TokenType::RETURNIF:
      return "returnif";
    case TokenType::IF:
      return "if";
    case TokenType::ELSE:
      return "else";
    case TokenType::OR:
      return "or";
    case TokenType::AND:
      return "and";
    case TokenType::TRUE:
      return "true";
    case TokenType::FALSE:
      return "false";
    case TokenType::NIL:
      return "nil";
    case TokenType::THIS:
      return "this";
    case TokenType::ERROR:
      return "ERROR";
    case TokenType::END:
      return "END";
    default:
      return "UNKNOWN";
  }
}

TEST_CASE("Semicolon insertion tests") {
  std::vector<SemicolonTest> semicolonTests = {
      {"", ""},
      {";", ";"},
      {"foo\n", "IDENT ;"},
      {"123\n", "INTEGER ;"},
      {"1.2\n", "DOUBLE ;"},
      {"\"x\"\n", "STRING ;"},
      {"true\n", "true ;"},
      {"false\n", "false ;"},
      {"nil\n", "nil ;"},
      {"this\n", "this ;"},
      {")\n", ") ;"},
      {"}\n", "} ;"},
      {"+\n", "+"},
      {"-\n", "-"},
      {"*\n", "*"},
      {"/\n", "/"},
      {"and\n", "and"},
      {"bool\n", "bool"},
      {"double\n", "double"},
      {"else\n", "else"},
      {"fn\n", "fn"},
      {"in\n", "in"},
      {"interface\n", "interface"},
      {"struct\n", "struct"},
      {"returnif\n", "returnif"},
      {"if\n", "if"},
      {"let\n", "let"},
      {"or\n", "or"},
  };

  for (const auto& test : semicolonTests) {
    Tokenizer tokenizer(test.input);
    std::string actualTokens;
    std::unique_ptr<Token> token;

    do {
      token = std::make_unique<Token>(tokenizer.scanToken());
      if (token->type != TokenType::END) {
        actualTokens += tokenTypeToString(token->type);
        actualTokens += " ";
      }
    } while (token->type != TokenType::END);

    if (!actualTokens.empty()) {
      actualTokens.pop_back();
    }

    REQUIRE(actualTokens == test.expectedTokens);
  }
}

TEST_CASE("More complex code is tokenized correctly", "[tokenizer]") {
  SECTION("keyword interface") {
    Tokenizer sut("interface Interface {}");

    auto keywordInterfaceToken = sut.scanToken();

    REQUIRE(keywordInterfaceToken.type == TokenType::INTERFACE);
    REQUIRE(keywordInterfaceToken.line == 1);
    REQUIRE(keywordInterfaceToken.lexeme == "interface");
  }

  SECTION("name of an interface") {
    Tokenizer sut("interface \nInterface {}");
    skipTokens(sut, 1);

    auto identifier = sut.scanToken();

    REQUIRE(identifier.type == TokenType::IDENTIFIER);
    REQUIRE(identifier.line == 2);
    REQUIRE(identifier.lexeme == "Interface");
  }

  SECTION("left bracket") {
    Tokenizer sut("interface \nInterface {\n}");
    skipTokens(sut, 2);

    auto brace = sut.scanToken();

    REQUIRE(brace.type == TokenType::LEFT_BRACE);
    REQUIRE(brace.line == 2);
    REQUIRE(brace.lexeme == "{");
  }

  SECTION("right bracket") {
    Tokenizer sut("interface \nInterface {\n}");
    skipTokens(sut, 3);

    auto brace = sut.scanToken();

    REQUIRE(brace.type == TokenType::RIGHT_BRACE);
    REQUIRE(brace.line == 3);
    REQUIRE(brace.lexeme == "}");
  }

  SECTION("string") {
    Tokenizer sut("\"string\"");

    auto brace = sut.scanToken();

    REQUIRE(brace.type == TokenType::STRING);
    REQUIRE(brace.line == 1);
    REQUIRE(brace.lexeme == "\"string\"");
  }

  SECTION("integer") {
    Tokenizer sut("1");

    auto brace = sut.scanToken();

    REQUIRE(brace.type == TokenType::INTEGER);
    REQUIRE(brace.line == 1);
    REQUIRE(brace.lexeme == "1");
  }

  SECTION("double") {
    Tokenizer sut("1.1");

    auto brace = sut.scanToken();

    REQUIRE(brace.type == TokenType::DOUBLE);
    REQUIRE(brace.line == 1);
    REQUIRE(brace.lexeme == "1.1");
  }

  SECTION("ignores whitespace") {
    Tokenizer sut("     1.1");

    auto brace = sut.scanToken();

    REQUIRE(brace.type == TokenType::DOUBLE);
    REQUIRE(brace.line == 1);
    REQUIRE(brace.lexeme == "1.1");
  }

  SECTION("function") {
    Tokenizer sut("fn check (int var): bool {\nreturn true\n}");

    auto fn = sut.scanToken();
    auto fnName = sut.scanToken();
    auto leftParen = sut.scanToken();
    auto parType = sut.scanToken();
    auto parName = sut.scanToken();
    auto rightParen = sut.scanToken();
    auto colon = sut.scanToken();
    auto returnType = sut.scanToken();
    auto leftBrace = sut.scanToken();
    auto returnKeyword = sut.scanToken();
    auto trueKeyword = sut.scanToken();
    auto autoSemicolon = sut.scanToken();
    auto rightBrace = sut.scanToken();

    REQUIRE(fn.type == TokenType::FN);
    REQUIRE(fn.line == 1);
    REQUIRE(fn.lexeme == "fn");

    REQUIRE(fnName.type == TokenType::IDENTIFIER);
    REQUIRE(fnName.line == 1);
    REQUIRE(fnName.lexeme == "check");

    REQUIRE(leftParen.type == TokenType::LEFT_PAREN);
    REQUIRE(leftParen.line == 1);
    REQUIRE(leftParen.lexeme == "(");

    REQUIRE(parType.type == TokenType::TYPE_INTEGER);
    REQUIRE(parType.line == 1);
    REQUIRE(parType.lexeme == "int");

    REQUIRE(parName.type == TokenType::IDENTIFIER);
    REQUIRE(parName.line == 1);
    REQUIRE(parName.lexeme == "var");

    REQUIRE(rightParen.type == TokenType::RIGHT_PAREN);
    REQUIRE(rightParen.line == 1);
    REQUIRE(rightParen.lexeme == ")");

    REQUIRE(colon.type == TokenType::COLON);
    REQUIRE(colon.line == 1);
    REQUIRE(colon.lexeme == ":");

    REQUIRE(returnType.type == TokenType::TYPE_BOOL);
    REQUIRE(returnType.line == 1);
    REQUIRE(returnType.lexeme == "bool");

    REQUIRE(leftBrace.type == TokenType::LEFT_BRACE);
    REQUIRE(leftBrace.line == 1);
    REQUIRE(leftBrace.lexeme == "{");

    REQUIRE(returnKeyword.type == TokenType::RETURN);
    REQUIRE(returnKeyword.line == 2);
    REQUIRE(returnKeyword.lexeme == "return");

    REQUIRE(trueKeyword.type == TokenType::TRUE);
    REQUIRE(trueKeyword.line == 2);
    REQUIRE(trueKeyword.lexeme == "true");

    REQUIRE(autoSemicolon.type == TokenType::SEMICOLON);
    REQUIRE(autoSemicolon.line == 2);
    REQUIRE(autoSemicolon.lexeme == "\n");

    REQUIRE(rightBrace.type == TokenType::RIGHT_BRACE);
    REQUIRE(rightBrace.line == 3);
    REQUIRE(rightBrace.lexeme == "}");
  }
}

TEST_CASE("Tokenizer auto inserts semicolons:", "[tokenizer]") {
  SECTION("recognizes LF after a string as statement end") {
    Tokenizer sut("let name = \"onol\"\nlet age = 0\nif(age > 1)\n{}");
    skipTokens(sut, 4);

    auto endStatement = sut.scanToken();

    REQUIRE(endStatement.type == TokenType::SEMICOLON);
    REQUIRE(endStatement.line == 1);
    REQUIRE(endStatement.lexeme == "\n");
  }

  SECTION("recognizes LF after a number as statement end") {
    Tokenizer sut("let name = \"onol\"\nlet age = 0\nif(age > 1)\n{}");
    skipTokens(sut, 9);

    auto endStatement = sut.scanToken();

    REQUIRE(endStatement.type == TokenType::SEMICOLON);
    REQUIRE(endStatement.line == 2);
    REQUIRE(endStatement.lexeme == "\n");
  }

  SECTION("recognizes CRLF as statement end") {
    Tokenizer sut("let name = \"onol\"\r\nlet age = 0\r\nif(age > 1)\r\n{}");
    skipTokens(sut, 4);

    auto endStatement = sut.scanToken();

    REQUIRE(endStatement.type == TokenType::SEMICOLON);
    REQUIRE(endStatement.line == 1);
    REQUIRE(endStatement.lexeme == "\n");
  }

  SECTION("ignores LF if it is not valid statement end") {
    Tokenizer sut("let name = \"onol\"\nlet age = 0\nif (age > 1) {\n}");
    skipTokens(sut, 16);

    auto leftBrace = sut.scanToken();
    auto rightBrace = sut.scanToken();

    REQUIRE(leftBrace.type == TokenType::LEFT_BRACE);
    REQUIRE(leftBrace.line == 3);
    REQUIRE(leftBrace.lexeme == "{");

    REQUIRE(rightBrace.type == TokenType::RIGHT_BRACE);
    REQUIRE(rightBrace.line == 4);
    REQUIRE(rightBrace.lexeme == "}");
  }
}
