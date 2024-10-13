#include <memory>
#include "bytecode.hpp"
#include "vm.hpp"

int main() {
  std::shared_ptr<Bytecode> bytecode = std::make_shared<Bytecode>();
  VM vm;

  bytecode->putConstant(42, 1);
  bytecode->putConstant(10, 1);
  bytecode->putOpCode(OpCode::OP_ADD, 1);

  bytecode->putConstant(39, 1);
  bytecode->putOpCode(OpCode::OP_SUBTRACT, 1);
  bytecode->putConstant(12, 1);
  bytecode->putOpCode(OpCode::OP_SUBTRACT, 1);

  bytecode->putConstant(2.1, 1);
  bytecode->putOpCode(OpCode::OP_MULTIPLY, 1);

  bytecode->putConstant(2, 1);
  bytecode->putOpCode(OpCode::OP_DIVIDE, 1);

  bytecode->putOpCode(OpCode::OP_RETURN, 2);

  vm.interpret(bytecode);

  bytecode->free();
  return 0;
}
