#include "compiler.hpp"
#include <iomanip>

#include "tokenizer.hpp"

void Compiler::compile(std::string_view sourceCode) {
  Tokenizer tokenizer(sourceCode);
  int line = -1;

  for (;;) {
    Token token = tokenizer.scanToken();
    if (token.line != line) {
      std::cout << token.line;
      line = token.line;
    } else {
      std::cout << "   | ";
    }
    std::cout << std::setw(2) << static_cast<int>(token.type) << " '"
              << token.lexeme << "'\n";

    if (token.type == TokenType::END) {
      break;
    }
  }
}
