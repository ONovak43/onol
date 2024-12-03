#include <filesystem>
#include <fstream>
#include <iostream>

#include "vm.hpp"

VM vm;

static std::string readFile(const char* path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cerr << "Could not open file " << path << ".\n";
    exit(74);
  }

  std::size_t fileSize = std::filesystem::file_size(path);
  std::string content(fileSize, '\0');

  file.read(content.data(), fileSize);
  return content;
}


static void repl() {
  std::string line;

  while (true) {
    std::cout << "> ";

    if (!std::getline(std::cin, line)) {
      std::cout << "\n";
      break;
    }

    vm.interpret(line);
  }
}

static void runFile(const char* path) {
  std::string source = readFile(path);
  InterpretResult result = vm.interpret(source);

  if (result == InterpretResult::INTERPRET_COMPILE_ERROR) exit(65);
  if (result == InterpretResult::INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    std::cerr << "Usage: onol [path]\n";
    exit(64);
  }

  return 0;
}
