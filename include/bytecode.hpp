#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>

#include "allocator.hpp"
#include "types.hpp"

enum class OpCode {
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_RETURN
};

class Bytecode {
 private:
  struct LineStart {
    std::size_t offset;
    uint32_t line;
  };

  std::vector<std::uint8_t, Allocator<std::uint8_t>> code;
  std::vector<Type, Allocator<Type>> constantPool;
  std::vector<std::unique_ptr<LineStart>> lines;
  void addLine(uint32_t line);

 public:
  Bytecode() = default;
  void putRaw(uint8_t byte, uint32_t line);
  void putOpCode(OpCode byte, uint32_t line);
  std::size_t putConstant(Type value, uint32_t line);
  void free();
  OpCode getOpCode(int index);
  uint8_t getConstantAddress(int index);
  Type getConstant(int address);
  uint32_t getLine(std::size_t address);
  std::size_t count();
};
