#pragma once

#include "bytecode.hpp"

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

void disassembleBytecode(Bytecode& bytecode, const std::string& name);
int disassembleInstruction(Bytecode& bytecode, uint32_t offset);
