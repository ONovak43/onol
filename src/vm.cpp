#include "vm.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <variant>

#include "bytecode.hpp"
#include "debug.hpp"
#include "types.hpp"

static bool valuesEqual(Type a, Type b) {
  if (!isSameType(a, b)) {
    return false;
  }

  return std::visit(
      [](auto&& argA, auto&& argB) -> bool {
        using T1 = std::decay_t<decltype(argA)>;
        using T2 = std::decay_t<decltype(argB)>;

        if constexpr (std::is_same_v<T1, T2>) {
          if constexpr (std::is_same_v<T1, Null>) {
            return true;
          } else if constexpr (std::is_pointer_v<T1> &&
                               std::is_base_of_v<Object,
                                                 std::remove_pointer_t<T1>>) {
            if (!argA || !argB) {
              return false;
            }

            auto objStringA = dynamic_cast<ObjString*>(argA);
            auto objStringB = dynamic_cast<ObjString*>(argB);

            if (objStringA && objStringB) {
              return objStringA->value == objStringB->value;
            }

            return false;
          } else {
            return argA == argB;
          }
        } else {
          return false;
        }
      },
      a, b);
}

Type VM::pop() {
  Type stackValue = stack.back();
  stack.pop_back();
  return stackValue;
}

void VM::push(Type value) {
  stack.push_back(value);
}

std::size_t VM::currentInstructionAddress() {
  return ip - bytecode->getCodePointer();
}

uint32_t VM::getCurrentLine() {
  return currentInstructionAddress() - 1;
}

String VM::toString(const Type& value) {
  return std::visit(
      [](auto&& arg) -> String {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, String>) {
          return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, Null>) {
          return "null";
        } else if constexpr (std::is_same_v<T, Object>) {
          auto objStr = dynamic_cast<ObjString*>(arg);

          if (objStr) {
            return objStr->value;
          }
          return "object";
        } else {
          return "unkown";
        }
      },
      value);
}

VM::~VM() {
  for (Object* obj : objects) {
    if (obj) {
      destructAndDeallocate(obj);
    }
  }
  objects.clear();
}

InterpretResult VM::interpret(const std::string& sourceCode) {
  bytecode = std::make_shared<Bytecode>();

  if (!parser.parse(sourceCode, bytecode)) {
    bytecode->free();
    return InterpretResult::INTERPRET_COMPILE_ERROR;
  }

  ip = bytecode->getCodePointer();
  InterpretResult result;

  try {
    result = run();
  } catch (const RuntimeError& ex) {
    std::cerr << ex.what() << "\n";
  }

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
    disassembleInstruction(*bytecode, currentInstructionAddress());
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
              } else {
                throw RuntimeError(getCurrentLine(),
                                   "Operand must be a number.");
              }
            },
            value);
        break;
      }
      case OpCode::NOT: {
        Type val = pop();
        if (isBool(val)) {
          bool boolVal = asBool(val);
          push(!boolVal);
        } else {
          throw RuntimeError(getCurrentLine(),
                             "Operand must be a boolean value.");
        }
        break;
      }
      case OpCode::RETURN: {
        printValue(pop());
        std::cout << "\n";
        return InterpretResult::INTERPRET_OK;
      }
      case OpCode::FALSE: {
        push(false);
        break;
      }
      case OpCode::TRUE: {
        push(true);
        break;
      }
      case OpCode::EQUAL: {
        Type b = pop();
        Type a = pop();
        push(valuesEqual(a, b));
        break;
      }
      case OpCode::GREATER: {
        binaryOp(std::greater<>());
        break;
      }
      case OpCode::GREATER_EQUAL: {
        binaryOp(std::greater_equal<>());
        break;
      }
      case OpCode::LESS: {
        binaryOp(std::less<>());
        break;
      }
      case OpCode::LESS_EQUAL: {
        binaryOp(std::less_equal<>());
        break;
      }
      case OpCode::NOT_EQUAL: {
        Type b = pop();
        Type a = pop();
        push(!valuesEqual(a, b));
        break;
      }
      case OpCode::NUL: {
        push(Null{});
        break;
      }
      default: {
        break;
      }
    }
  }
  return InterpretResult::INTERPRET_OK;
}
