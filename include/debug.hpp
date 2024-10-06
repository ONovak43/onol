#pragma once

#include "bytecode.hpp"

#ifdef DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

void dissasembleBytecode(Bytecode& bytecode, const std::string& name);
int dissasembleInstruction(Bytecode& bytecode, uint32_t offset);

