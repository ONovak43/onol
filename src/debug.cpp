#include "debug.hpp"

#include <iomanip>
#include <ostream>

#include "bytecode.hpp"
#include "types.hpp"

static std::size_t simpleInstruction(const std::string& name,
                                     std::size_t offset) {
  std::cout << name << "\n";
  return offset + 1;
}

static std::size_t constantInstruction(const std::string& name,
                                       Bytecode& bytecode, std::size_t offset) {
  uint8_t constantAddress = bytecode.getConstantAddress(offset + 1);

  std::cout << std::left << std::setw(16) << name << std::setw(4)
            << static_cast<int>(constantAddress) << "'";
  std::cout << std::setfill(' ') << std::setw(0);
  printValue(bytecode.getConstant(constantAddress));
  std::cout << "'" << "\n";
  return offset + 2;
}

static std::size_t constantLongInstruction(const std::string& name,
                                           Bytecode& bytecode,
                                           std::size_t offset) {
  uint32_t constantAddress = bytecode.getConstantAddress(offset + 1) |
                             (bytecode.getConstantAddress(offset + 2) << 8) |
                             (bytecode.getConstantAddress(offset + 3) << 16);

  std::cout << std::left << std::setw(16) << name << std::setw(4)
            << static_cast<int>(constantAddress) << "'";
  std::cout << std::setfill(' ') << std::setw(0);
  printValue(bytecode.getConstant(constantAddress));
  std::cout << "'" << "\n";
  return offset + 4;
}

void disassembleBytecode(Bytecode& bytecode, const std::string& name) {
  std::cout << "== " << name << " ==\n";

  for (std::size_t offset = 0; offset < bytecode.count();) {
    offset = disassembleInstruction(bytecode, offset);
  }
}

int disassembleInstruction(Bytecode& bytecode, uint32_t offset) {
  std::cout << std::setfill('0') << std::setw(4) << std::right << offset << " ";

  if (offset > 0 && bytecode.getLine(offset) == bytecode.getLine(offset - 1)) {
    std::cout << "   | ";
  } else {
    std::cout << std::setw(4) << std::right << bytecode.getLine(offset) << " ";
  }

  std::cout << std::setfill(' ');

  OpCode opCode = bytecode.getOpCode(offset);
  switch (opCode) {
    case OpCode::CONSTANT:
      return constantInstruction("CONSTANT", bytecode, offset);
    case OpCode::CONSTANT_LONG:
      return constantLongInstruction("CONSTANT_LONG", bytecode, offset);
    case OpCode::DEFINE_GLOBAL:
      return constantInstruction("DEFINE_GLOBAL", bytecode, offset);
    case OpCode::DEFINE_GLOBAL_LONG:
      return constantLongInstruction("DEFINE_GLOBAL_LONG", bytecode, offset);
    case OpCode::GET_GLOBAL:
      return constantInstruction("GET_GLOBAL", bytecode, offset);
    case OpCode::GET_GLOBAL_LONG:
      return constantLongInstruction("GET_GLOBAL_LONG", bytecode, offset);
    case OpCode::SET_GLOBAL:
      return constantInstruction("SET_GLOBAL", bytecode, offset);
    case OpCode::SET_GLOBAL_LONG:
      return constantLongInstruction("SET_GLOBAL_LONG", bytecode, offset);
    case OpCode::FALSE:
      return simpleInstruction(std::string("FALSE"), offset);
    case OpCode::TRUE:
      return simpleInstruction(std::string("TRUE"), offset);
    case OpCode::NUL:
      return simpleInstruction(std::string("NULL"), offset);
    case OpCode::ADD:
      return simpleInstruction("ADD", offset);
    case OpCode::SUBTRACT:
      return simpleInstruction("SUBTRACT", offset);
    case OpCode::MULTIPLY:
      return simpleInstruction("MULTIPLY", offset);
    case OpCode::DIVIDE:
      return simpleInstruction("DIVIDE", offset);
    case OpCode::NEGATE:
      return simpleInstruction("NEGATE", offset);
    case OpCode::NOT:
      return simpleInstruction("NOT", offset);
    case OpCode::EQUAL:
      return simpleInstruction("EQUAL", offset);
    case OpCode::GREATER:
      return simpleInstruction("GREATER", offset);
    case OpCode::GREATER_EQUAL:
      return simpleInstruction("GREATER_EQUAL", offset);
    case OpCode::LESS:
      return simpleInstruction("LESS", offset);
    case OpCode::LESS_EQUAL:
      return simpleInstruction("LESS_EQUAL", offset);
    case OpCode::NOT_EQUAL:
      return simpleInstruction("NOT_EQUAL", offset);
    case OpCode::POP:
      return simpleInstruction("OP_POP", offset);
    case OpCode::RETURN:
      return simpleInstruction(std::string("RETURN"), offset);
    default:
      std::cout << "Unkown opcode " << static_cast<uint8_t>(opCode) << "\n";
      return offset + 1;
  }
}
