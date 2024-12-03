#include "vm.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <variant>

#include "bytecode.hpp"
#include "debug.hpp"
#include "types.hpp"

Type VM::pop() {
  Type stackValue = stack.back();
  stack.pop_back();
  return stackValue;
}

void VM::push(Type value) {
  stack.push_back(value);
}

InterpretResult VM::interpret(const std::string& sourceCode) {
  bytecode = std::make_shared<Bytecode>();

  if (!parser.parse(sourceCode, bytecode)) {
    bytecode->free();
    return InterpretResult::INTERPRET_COMPILE_ERROR;
  }

  ip = bytecode->getCodePointer();
  InterpretResult result;

  result = run();

  return result;
}

InterpretResult VM::run() {
  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    std::cout << "           ";
    for (const auto& slot : stack) {
      std::cout << "[ ";
      printValue(slot);
      std::cout << " ]";
    }
    std::cout << "\n";
    disassembleInstruction(*bytecode, (ip - bytecode->getCodePointer()));
#endif
    uint8_t instruction = readByte();
    switch (static_cast<OpCode>(instruction)) {
      case OpCode::CONSTANT: {
        Type constant = readConstant();
        push(constant);
        break;
      }
      case OpCode::CONSTANT_LONG: {
        uint32_t constantAddr = 0;
        constantAddr |= readByte();
        constantAddr |= readByte() << 8;
        constantAddr |= readByte() << 16;

        Type constant = bytecode->getConstant(constantAddr);
        printValue(constant);
        std::cout << "\n";
        break;
      }
      case OpCode::ADD: {
        binaryOp(std::plus<>());
        break;
      }
      case OpCode::SUBTRACT: {
        binaryOp(std::minus<>());
        break;
      }
      case OpCode::MULTIPLY: {
        binaryOp(std::multiplies<>());
        break;
      }
      case OpCode::DIVIDE: {
        binaryOp(std::divides<>());
        break;
      }
      case OpCode::NEGATE: {
        Type value = pop();
        std::visit(
            [this](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, int32_t> ||
                            std::is_same_v<T, double>) {
                push(-arg);
              }
            },
            value);  // TODO fix unsupported types
        break;
      }
      case OpCode::RETURN: {
        printValue(pop());
        std::cout << "\n";
        return InterpretResult::INTERPRET_OK;
      } break;
    }
  }
  return InterpretResult::INTERPRET_OK;
}
