#include "bytecode.hpp"
#include "debug.hpp"

#define DEBUG

int main() {
  Bytecode bytecode;

  Type intValue = 42;

  bytecode.putOpCode(OpCode::OP_CONSTANT, 1);
  bytecode.putConstant(intValue, 1);
  std::cout << "Line 1: " << bytecode.getLine(0) << "\n";
  std::cout << "Line 1: " << bytecode.getLine(1) << "\n";
  bytecode.putOpCode(OpCode::OP_RETURN, 2);
  std::cout << "Line 2: " << bytecode.getLine(2) << "\n";
  bytecode.putOpCode(OpCode::OP_RETURN, 3);
  std::cout << "Line 3: " << bytecode.getLine(3) << "\n";
  bytecode.putOpCode(OpCode::OP_RETURN, 3);
  std::cout << "Line 3: " << bytecode.getLine(3) << "\n";
  dissasembleBytecode(bytecode, "test chunk");

  bytecode.free();
  return 0;
}
