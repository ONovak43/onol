#include "vm.hpp"

#include <cstdint>
#include <functional>
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

InterpretResult VM::interpret(std::shared_ptr<Bytecode> bcode) {
  bytecode = bcode;
  ip = bytecode->getCodePointer();
  return run();
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
    dissasembleInstruction(*bytecode, (ip - bytecode->getCodePointer()));
#endif
    uint8_t instruction = readByte();
    switch (static_cast<OpCode>(instruction)) {
      case OpCode::OP_CONSTANT: {
        Type constant = readConstant();
        push(constant);
        break;
      }
      case OpCode::OP_CONSTANT_LONG: {
        uint32_t constantAddr = 0;
        constantAddr |= readByte();
        constantAddr |= readByte() << 8;
        constantAddr |= readByte() << 16;

        Type constant = bytecode->getConstant(constantAddr);
        printValue(constant);
        std::cout << "\n";
        break;
      }
      case OpCode::OP_ADD: {
        binaryOp(std::plus<>());
        break;
      }
      case OpCode::OP_SUBTRACT: {
        binaryOp(std::minus<>());
        break;
      }
      case OpCode::OP_MULTIPLY: {
        binaryOp(std::multiplies<>());
        break;
      }
      case OpCode::OP_DIVIDE: {
        binaryOp(std::divides<>());
        break;
      }
      case OpCode::OP_NEGATE: {
        Type value = pop();
        std::visit(
            [this](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, double>) {
                push(-arg);
              }
            },
            value);  // TODO fix unsupported types
        break;
      }
      case OpCode::OP_RETURN: {
        printValue(pop());
        std::cout << "\n";
        return InterpretResult::INTERPRET_OK;
      } break;
    }
  }
  return InterpretResult::INTERPRET_OK;
}
