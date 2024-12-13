#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "bytecode.hpp"
#include "parser.hpp"

TEST_CASE("Valid expressions are parsed correctly", "[parser]") {
  Parser parser;
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();

  SECTION("Simple arithmetic expressions") {
    std::string source = "1 + 2";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Grouping with parentheses") {
    std::string source = "(1 + 2) * 3";
    REQUIRE(parser.parse(source, bytecode) == true);
  }
}

TEST_CASE("Invalid expressions cause parsing errors", "[parser]") {
  Parser parser;
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();

  SECTION("Mismatched parentheses") {
    std::string source = "(1 + 2";
    auto t = parser.parse(source, bytecode);
    REQUIRE(parser.parse(source, bytecode) == false);
  }

  SECTION("Unexpected tokens") {
    std::string source = "1 + + 2";
    REQUIRE(parser.parse(source, bytecode) == false);
  }
}

TEST_CASE("Numeric literals are parsed correctly", "[parser]") {
  Parser parser;
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();

  SECTION("Integer literals") {
    std::string source = "42";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Double literals") {
    std::string source = "3.14";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Out of range integer literal") {
    std::string source = "999999999999999999999999999999999";
    REQUIRE(parser.parse(source, bytecode) == false);
  }
}

TEST_CASE("Comparison operators are parsed correctly", "[parser]") {
  Parser parser;
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();

  SECTION("Greater than") {
    std::string source = "5 > 3";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Less than") {
    std::string source = "2 < 4";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Greater than or equal") {
    std::string source = "7 >= 7";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Less than or equal") {
    std::string source = "8 <= 10";
    REQUIRE(parser.parse(source, bytecode) == true);
  }
}

TEST_CASE("Invalid comparison expressions cause parsing errors", "[parser]") {
  Parser parser;
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();

  SECTION("Missing right operand") {
    std::string source = "5 > ";
    REQUIRE(parser.parse(source, bytecode) == false);
  }

  SECTION("Invalid token in comparison") {
    std::string source = "3 <= +";
    REQUIRE(parser.parse(source, bytecode) == false);
  }
}

TEST_CASE("Combined arithmetic and comparison expressions are parsed correctly",
          "[parser]") {
  Parser parser;
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();

  SECTION("Arithmetic with comparison") {
    std::string source = "(1 + 2) > (3 - 4)";
    REQUIRE(parser.parse(source, bytecode) == true);
  }

  SECTION("Multiple operators") {
    std::string source = "1 + 2 > 3 - 4";
    REQUIRE(parser.parse(source, bytecode) == true);
  }
}