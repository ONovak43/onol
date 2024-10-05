#include <bytecode.hpp>
#include <cstdint>

void Bytecode::put(uint8_t byte) {
    code.push_back(byte);
}
