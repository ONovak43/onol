#include "bytecode.hpp"

#include <memory>

void Bytecode::addLine(uint32_t line) {
  if (lines.size() > 0 && lines.back()->line == line) {
    return;
  }

  std::unique_ptr<LineStart> lineStart = std::make_unique<LineStart>();
  lineStart->line = line;
  lineStart->offset = code.size() - 1;

  lines.push_back(std::move(lineStart));
}

void Bytecode::putOpCode(OpCode byte, uint32_t line) {
  code.push_back(static_cast<uint8_t>(byte));
  addLine(line);
}

void Bytecode::putRaw(uint8_t byte, uint32_t line) {
  code.push_back(byte);
  addLine(line);
}

std::size_t Bytecode::putConstant(Type value, uint32_t line) {
  constantPool.push_back(value);

  std::size_t constantAddress = constantPool.size() - 1;
  if (constantAddress < 256) {
    putRaw(static_cast<uint8_t>(OpCode::OP_CONSTANT), line);
    putRaw(constantAddress, line);
  } else {
    putRaw(static_cast<uint8_t>(OpCode::OP_CONSTANT_LONG), line);
    putRaw(static_cast<uint8_t>(constantAddress & 0xff), line);  // little-endian
    putRaw(static_cast<uint8_t>((constantAddress >> 8) & 0xff), line);
    putRaw(static_cast<uint8_t>((constantAddress >> 16) & 0xff), line);
  }

  return constantAddress;
}

void Bytecode::free() {
  code.clear();
  code.shrink_to_fit();

  constantPool.clear();
  constantPool.shrink_to_fit();
}

OpCode Bytecode::getOpCode(int offset) {
  return static_cast<OpCode>(code[offset]);
}

uint8_t Bytecode::getConstantAddress(int offset) {
  return code[offset];
}

Type Bytecode::getConstant(int address) {
  return constantPool[address];
}

uint32_t Bytecode::getLine(std::size_t offset) {
  if (lines.empty()) {
    return 0;
  }

  for (std::size_t i = lines.size(); i > 0; --i) {
    if (lines[i - 1]->offset <= offset) {
      return lines[i - 1]->line;
    }
  }
  return 0;
}

uint8_t* Bytecode::getCodePointer() {
  return code.data();
}

std::size_t Bytecode::count() {
  return code.size();
}
