#include "debug.hpp"

#include <iomanip>
#include <ostream>

#include "types.hpp"

static std::size_t simpleInstruction(const std::string& name, std::size_t offset) {
  std::cout << name << "\n";
  return offset + 1;
}

static std::size_t constantInstruction(const std::string& name, Bytecode& bytecode, std::size_t offset) {
  uint8_t constantAddress = bytecode.getConstantAddress(offset + 1);

  std::cout << std::left << std::setw(16) << name << std::setw(4) << static_cast<int>(constantAddress) << "'";
  std::cout << std::setfill(' ') << std::setw(0);
  printValue(bytecode.getConstant(constantAddress));
  std::cout << "'" << "\n";
  return offset + 2;
}

static std::size_t constantLongInstruction(const std::string& name, Bytecode& bytecode, std::size_t offset) {
  uint32_t constantAddress = bytecode.getConstantAddress(offset + 1)
    | (bytecode.getConstantAddress(offset + 2) << 8)
    | (bytecode.getConstantAddress(offset + 3) << 16);

  std::cout << std::left << std::setw(16) << name << std::setw(4) << static_cast<int>(constantAddress) << "'";
  std::cout << std::setfill(' ') << std::setw(0);
  printValue(bytecode.getConstant(constantAddress));
  std::cout << "'" << "\n";
  return offset + 4;
}

void dissasembleBytecode(Bytecode& bytecode, const std::string& name) {
  std::cout << "== " << name << " ==\n";

  for (std::size_t offset = 0; offset < bytecode.count();) {
    offset = dissasembleInstruction(bytecode, offset);
  }
}

int dissasembleInstruction(Bytecode& bytecode, uint32_t offset) {
  std::cout << std::setfill('0') << std::setw(4) << std::right << offset << " ";

  if (offset > 0 && bytecode.getLine(offset) == bytecode.getLine(offset - 1)) {
    std::cout << "   | ";
  } else {
    std::cout << std::setw(4) << std::right << bytecode.getLine(offset) << " ";
  }

  std::cout << std::setfill(' ');

  OpCode opCode = bytecode.getOpCode(offset);
  switch (opCode) {
    case OpCode::OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", bytecode, offset);
    case OpCode::OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", bytecode, offset);
    case OpCode::OP_RETURN:
      return simpleInstruction(std::string("OP_RETURN"), offset);
    default:
      std::cout << "Unkown opcode " << static_cast<uint8_t>(opCode) << "\n";
      return offset + 1;
  }
}
