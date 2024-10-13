#pragma once

#include "bytecode.hpp"

#define DEBUG_TRACE_EXECUTION

void dissasembleBytecode(Bytecode& bytecode, const std::string& name);
int dissasembleInstruction(Bytecode& bytecode, uint32_t offset);
