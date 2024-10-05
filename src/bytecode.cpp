#include <bytecode.hpp>

void Bytecode::put(OpCode byte) { code.push_back(static_cast<uint8_t>(byte)); }

void Bytecode::free() {
  code.clear();
  code.shrink_to_fit();
}
