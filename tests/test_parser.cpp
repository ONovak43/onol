#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "bytecode.hpp"

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
