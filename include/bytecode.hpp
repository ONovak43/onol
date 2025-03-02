#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "allocator.hpp"
#include "types.hpp"

enum class OpCode : uint8_t {
  CONSTANT,
  CONSTANT_LONG,
  DEFINE_GLOBAL,
  DEFINE_GLOBAL_LONG,
  GET_GLOBAL,
  GET_GLOBAL_LONG,
  SET_GLOBAL,
  SET_GLOBAL_LONG,
  NUL,
  TRUE,
  FALSE,
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  NEGATE,
  NOT,
  EQUAL,
  GREATER,
  LESS,
  GREATER_EQUAL,
  LESS_EQUAL,
  NOT_EQUAL,
  POP,
  RETURN
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
  void putRaw(uint8_t byte, uint32_t line);
  void putRaw(std::size_t byte, uint32_t line);
  void putOpCode(OpCode byte, uint32_t line);
  std::size_t putConstant(Type value, uint32_t line);
  std::size_t createConstant(Type value);
  void free();
  OpCode getOpCode(int index);
  uint8_t getConstantAddress(int index);
  Type getConstant(int address);
  uint32_t getLine(std::size_t address);
  uint8_t* getCodePointer();
  std::size_t count();
};
